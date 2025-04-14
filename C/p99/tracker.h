#ifndef __TRACKER_H__
#define __TRACKER_H__

long current_time_us();

void tracker_init(double *percentiles, int count);
void tracker_insert(long value);
long tracker_query(double quantile);
void tracker_print_results();

#endif