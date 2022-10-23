#include <linux/kernel.h>
#include <linux/ktime.h>

int readPolicy = 0;
EXPORT_SYMBOL(readPolicy);

long tifa_bio_ttl; //Total number of bios sent in RAID
EXPORT_SYMBOL(tifa_bio_ttl);
long tifa_bio_ret; //Number of retry bios sent in RAID
EXPORT_SYMBOL(tifa_bio_ret);
long tifa_bio_rfw; //Number of bios read for write
EXPORT_SYMBOL(tifa_bio_rfw);
long tifa_bio_gct; //Number of gct bios sent in RAID
EXPORT_SYMBOL(tifa_bio_gct);
long tifa_bio_gct_ret; //Among retry bios, number of retry for gct
EXPORT_SYMBOL(tifa_bio_gct_ret);
long tifa_bio_rfw_ret; //Among retry bios, number of retry for rfw
EXPORT_SYMBOL(tifa_bio_rfw_ret);
long tifa_bio_com; //Number of bios computed
EXPORT_SYMBOL(tifa_bio_com);
long tifa_bio_gct_nor; //Number of bios gct normal finished
EXPORT_SYMBOL(tifa_bio_gct_nor);
long tifa_bio_gct_eio; //Number of gct returned with eio
EXPORT_SYMBOL(tifa_bio_gct_eio);
long tifa_bio_rfw_nor; //Number of bios rfw normal finished
EXPORT_SYMBOL(tifa_bio_rfw_nor);
long tifa_bio_rfw_eio; //Number of rfw returned with eio
EXPORT_SYMBOL(tifa_bio_rfw_eio);
long tifa_bio_stripe; //Number of full stripe
EXPORT_SYMBOL(tifa_bio_stripe);

atomic_t tifa_dio_ttl;
EXPORT_SYMBOL(tifa_dio_ttl);
atomic_t tifa_dio_gc;
EXPORT_SYMBOL(tifa_dio_gc);
atomic_t stripe_in_gc_num[5];
EXPORT_SYMBOL(stripe_in_gc_num);
atomic_t tifa_dio_in_gc_num[5];
EXPORT_SYMBOL(tifa_dio_in_gc_num);

u64 *rdlat_array = NULL;
EXPORT_SYMBOL(rdlat_array);
atomic_t rdlat_idx;
EXPORT_SYMBOL(rdlat_idx);
