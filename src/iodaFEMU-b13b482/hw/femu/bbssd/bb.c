#include "../nvme.h"
#include "./ftl.h"

static void bb_init_ctrl_str(FemuCtrl *n)
{
    static int fsid_vbb = 0;
    const char *vbbssd_mn = "FEMU BlackBox-SSD Controller";
    const char *vbbssd_sn = "vSSD";

    nvme_set_ctrl_name(n, vbbssd_mn, vbbssd_sn, &fsid_vbb);
}

/* bb <=> black-box */
static void bb_init(FemuCtrl *n, Error **errp)
{
    struct ssd *ssd = n->ssd = g_malloc0(sizeof(struct ssd));

    bb_init_ctrl_str(n);

    ssd->dataplane_started_ptr = &n->dataplane_started;
    ssd->ssdname = (char *)n->devname;
    femu_debug("Starting FEMU in Blackbox-SSD mode ...\n");
    ssd_init(n);
}

static void bb_flip(FemuCtrl *n, NvmeCmd *cmd)
{
    struct ssd *ssd = n->ssd;
    int64_t cdw10 = le64_to_cpu(cmd->cdw10);

    switch (cdw10) {
    case FEMU_ENABLE_GC_DELAY:
        ssd->sp.enable_gc_delay = true;
        femu_log("%s,FEMU GC Delay Emulation [Enabled]!\n", n->devname);
        break;
    case FEMU_DISABLE_GC_DELAY:
        ssd->sp.enable_gc_delay = false;
        femu_log("%s,FEMU GC Delay Emulation [Disabled]!\n", n->devname);
        break;
    case FEMU_ENABLE_DELAY_EMU:
        ssd->sp.pg_rd_lat = NAND_READ_LATENCY;
        ssd->sp.pg_wr_lat = NAND_PROG_LATENCY;
        ssd->sp.blk_er_lat = NAND_ERASE_LATENCY;
        ssd->sp.ch_xfer_lat = 0;
        femu_log("%s,FEMU Delay Emulation [Enabled]!\n", n->devname);
        break;
    case FEMU_DISABLE_DELAY_EMU:
        ssd->sp.pg_rd_lat = 0;
        ssd->sp.pg_wr_lat = 0;
        ssd->sp.blk_er_lat = 0;
        ssd->sp.ch_xfer_lat = 0;
        femu_log("%s,FEMU Delay Emulation [Disabled]!\n", n->devname);
        break;
    case FEMU_RESET_ACCT:
        n->nr_tt_ios = 0;
        n->nr_tt_late_ios = 0;
        femu_log("%s,Reset tt_late_ios/tt_ios,%lu/%lu\n", n->devname,
                n->nr_tt_late_ios, n->nr_tt_ios);
        break;
    case FEMU_ENABLE_LOG:
        n->print_log = true;
        femu_log("%s,Log print [Enabled]!\n", n->devname);
        break;
    case FEMU_DISABLE_LOG:
        n->print_log = false;
        femu_log("%s,Log print [Disabled]!\n", n->devname);
        break;

    case FEMU_SYNC_GC:
        ssd->sp.enable_gc_sync = true;
        femu_log("%s,FEMU GC Synchronization [Enabled]!\n", n->devname);
        break;
    case FEMU_UNSYNC_GC:
        ssd->sp.enable_gc_sync = false;
        femu_log("%s,FEMU GC Synchronization [Disabled]!\n", n->devname);
        break;

    case FEMU_WINDOW_1S:
        femu_log("FEMU_WINDOW_1S\n");
        ssd->sp.gc_sync_window = 1000;
        break;
    case FEMU_WINDOW_100MS:
        femu_log("FEMU_WINDOW_100MS\n");
        ssd->sp.gc_sync_window = 100;
        break;
    case FEMU_WINDOW_2S:
        femu_log("FEMU_WINDOW_2S\n");
        ssd->sp.gc_sync_window = 2000;
        break;
    case FEMU_WINDOW_10MS:
        femu_log("FEMU_WINDOW_10MS\n");
        ssd->sp.gc_sync_window = 10;
        break;
    case FEMU_WINDOW_40MS:
        femu_log("FEMU_WINDOW_40MS\n");
        ssd->sp.gc_sync_window = 40;
        break;
    case FEMU_WINDOW_200MS:
        femu_log("FEMU_WINDOW_200MS\n");
        ssd->sp.gc_sync_window = 200;
        break;
    case FEMU_WINDOW_400MS:
        femu_log("FEMU_WINDOW_400MS\n");
        ssd->sp.gc_sync_window = 400;
        break;

    case FEMU_ENABLE_HARMONIA:
        femu_log("FEMU_ENABLE_HARMONIA\n");
        ssd->sp.harmonia = true;
        break;
    case FEMU_DISABLE_HARMONIA:
        femu_log("FEMU_DISABLE_HARMONIA\n");
        ssd->sp.harmonia = false;
        break;

    case FEMU_PRINT_AND_RESET_COUNTERS:
        femu_log("FEMU_PRINT_AND_RESET_COUNTERS\n");
        femu_log("SSD%d: Total %d, 0GC %d, 1GC %d, 2GC %d, 3GC %d, 4GC %d\n",
                ssd->id, ssd->total_reads, ssd->num_reads_blocked_by_gc[0],
                ssd->num_reads_blocked_by_gc[1], ssd->num_reads_blocked_by_gc[2],
                ssd->num_reads_blocked_by_gc[3], ssd->num_reads_blocked_by_gc[4]);
        femu_log("SSD%d: 0GC %.4f, 1GC %.4f, 2GC %.4f, 3GC %.4f, 4GC %.4f\n",
                ssd->id,
                (float)ssd->num_reads_blocked_by_gc[0] * 100 / ssd->total_reads,
                (float)ssd->num_reads_blocked_by_gc[1] * 100 / ssd->total_reads,
                (float)ssd->num_reads_blocked_by_gc[2] * 100 / ssd->total_reads,
                (float)ssd->num_reads_blocked_by_gc[3] * 100 / ssd->total_reads,
                (float)ssd->num_reads_blocked_by_gc[4] * 100 / ssd->total_reads);
        ssd->total_reads = 0;
        ssd->num_reads_blocked_by_gc[0] = 0;
        ssd->num_reads_blocked_by_gc[1] = 0;
        ssd->num_reads_blocked_by_gc[2] = 0;
        ssd->num_reads_blocked_by_gc[3] = 0;
        ssd->num_reads_blocked_by_gc[4] = 0;
        break;

    case FEMU_NAND_UTILIZATION_LOG:
      ssd->nand_utilization_log = 1 - ssd->nand_utilization_log;
      femu_log("%s, FEMU_NAND_UTILIZATION_LOG TOGGLE, current value: %d\n",
               n->devname, ssd->nand_utilization_log);
      break;

    default:
        printf("FEMU:%s,Not implemented flip cmd (%lu)\n", n->devname, cdw10);
    }
}

static uint16_t bb_nvme_rw(FemuCtrl *n, NvmeNamespace *ns, NvmeCmd *cmd,
                           NvmeRequest *req)
{
    return nvme_rw(n, ns, cmd, req);
}

static uint16_t bb_io_cmd(FemuCtrl *n, NvmeNamespace *ns, NvmeCmd *cmd,
                          NvmeRequest *req)
{
    switch (cmd->opcode) {
    case NVME_CMD_READ:
    case NVME_CMD_WRITE:
        return bb_nvme_rw(n, ns, cmd, req);
    default:
        return NVME_INVALID_OPCODE | NVME_DNR;
    }
}

static uint16_t bb_admin_cmd(FemuCtrl *n, NvmeCmd *cmd)
{
    switch (cmd->opcode) {
    case NVME_ADM_CMD_FEMU_FLIP:
        bb_flip(n, cmd);
        return NVME_SUCCESS;
    default:
        return NVME_INVALID_OPCODE | NVME_DNR;
    }
}

int nvme_register_bbssd(FemuCtrl *n)
{
    n->ext_ops = (FemuExtCtrlOps) {
        .state            = NULL,
        .init             = bb_init,
        .exit             = NULL,
        .rw_check_req     = NULL,
        .admin_cmd        = bb_admin_cmd,
        .io_cmd           = bb_io_cmd,
        .get_log          = NULL,
    };

    return 0;
}

