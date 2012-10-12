let m="`date +%s` - 600"

ss=1349979500
ee=1349979563

rrdtool graph ip.png -t "MEP aggregation statistics" \
-v "# packets" \
--start $m \
 DEF:mep=aggregation.rrd:mep:AVERAGE \
 DEF:lost_meps=aggregation.rrd:lost_mep:AVERAGE \
 AREA:mep#0097a0:"meps" \
 GPRINT:mep:MAX:"\t\tmax\: %.2lf\n" 
#--start $ss --end $ee \
# GPRINT:mep:MAX:"\tmax\: %.21f" \
# GPRINT:lost_mep:MAX:"\tmax\: %.21f"
# LINE:mep#0099ff:"Received MEPs\n" \
# LINE:lost_meps#0000ff:"Lost MEPs\n"
