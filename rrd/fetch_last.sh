let m="`date +%s` - 60"

rrdtool fetch aggregation.rrd AVERAGE --start $m
