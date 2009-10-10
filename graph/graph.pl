//  set up plotting region using proc areadef
#proc areadef
    xrange: 0 10
    yrange: 0 11
    xaxis.stubs: inc 1
    yaxis.stubs: inc 1
    xaxis.label: Allocated Ratio
    yaxis.label: Observed Ratio

//  specify data using proc getdata
#proc getdata
    pathname: results.data

#proc scatterplot
    xfield: 1
    yfield: 2
    symbol: shape=pixcircle style=solid color=blue radius=0.03
    legendlabel: Observed

#proc getdata
    data:
        0 
        10

#proc lineplot
    xfield: 1
    yfield: 1
    linedetails: color=green width=0.5
    legendlabel: Ideal

#proc legend
    location: min+1 max-1
    format: down
    frame: width=0.5
    specifyorder:
        Observed
        Ideal
