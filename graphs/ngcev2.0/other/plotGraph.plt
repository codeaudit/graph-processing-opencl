set title "graph:ngce?modeltype=FullScaleFreeGraph&nodes=500&initialNodes=10&m=3&seed=0&version=2.0"
set term postscript eps
set output "../Plots/1291738995627.eps"
set logscale
set ylabel 'Connectivity Probability'
set xlabel 'Nodes' 
plot "NGCE_Output"
set term x11
plot "NGCE_Output"
pause 15 "Hit return to continue"#Analysis Completed in: 0.15 seconds