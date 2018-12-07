// Date handler beyond 4 digit years
// year value -14999 to 14999
function (bparams) {
    let dtstr = bparams.byear + ' ' + bparams.bmonth + ' ' 
        + bparams.bdate + ' ' + bparams.bhour + ':' 
        + bparams.bminute + ' GMT' + bparams.bzone;
    let yr1 = Number(bparams.byear);
    // handle two digit years differently
    // not as belonging to the current epoch
    if (yr1 >= 0 && yr1 <= 99) {
        dtstr = simpleZeroFill(yr1) + '-' + bparams.bmonth 
            + '-' + bparams.bdate;
    } 
    // if year is negative, ie before BC 
    // then date and month do not matter
    let bdt;
    if (yr1 > 0) bdt = new Date(dtstr); else bdt = new Date(yr1, 0);
        
}