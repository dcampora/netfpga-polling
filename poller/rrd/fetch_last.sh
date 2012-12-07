let m="`date +%s` - 24 * 3600"

rrdtool fetch aggregation.rrd AVERAGE --start $m
