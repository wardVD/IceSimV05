from math import log10
from I3Tray import I3Units
from math import isnan
from icecube import dataclasses as dc

def pmt_response_sim_test(omkey, cal, stat):

    if stat.pmt_hv <= 0 :
        print('  %s  pmt_hv = %s V !!' % (str(omkey), stat.pmt_hv/I3Units.V))
        return False    

    gain = cal.hv_gain_fit.slope * log10(stat.pmt_hv/I3Units.V) + cal.hv_gain_fit.intercept
    if isnan(stat.pmt_hv) or isnan(gain) :           
        print('  %s  pmt_hv = %s V !!' % (str(omkey), stat.pmt_hv/I3Units.V))
        print('  %s  hv gain = %s !!' % cal.hv_gain_fit.intercept)
        print('  %s  hv gain slope = %s !!' % cal.hv_gain_fit.slope)
        print('  %s  hv gain intercept = %s !!' % cal.hv_gain_fit.intercept)
        return False    
    return True

def dom_launcher_test(omkey, geo, cal, stat):

    success = True
    # pathological voltages will cause DOMLauncher to log_fatal
    if stat.pmt_hv < 0 or isnan(stat.pmt_hv) :
        print('  %s  pmt_hv = %s V !!' % (str(omkey), stat.pmt_hv/I3Units.V))        
        success = False

    # PMT threshold
    if geo.omtype == dc.I3OMGeo.OMType.IceTop :
        # check that both MPE and SPE thresholds are greater than 0
        if dc.old_mpe_threshold(stat) <= 0 or \
           isnan(dc.old_mpe_threshold(stat)) :
            thresh = dc.old_mpe_threshold(stat) 
            print('  %s  IceTop PMT MPE thresh = %s V !!' % (str(omkey), thresh/I3Units.V))
            success = False

        if dc.old_spe_threshold(stat) <= 0 or \
           isnan(dc.old_spe_threshold(stat)) :
            thresh = dc.old_spe_threshold(stat) 
            print('  %s  IceTop PMT SPE thresh = %s V !!' % (str(omkey), thresh/I3Units.V))
            success = False

    if geo.omtype == dc.I3OMGeo.OMType.IceCube :
        if dc.spe_pmt_threshold(stat, cal) <= 0 or \
           isnan(dc.old_mpe_threshold(stat)) :
            thresh = dc.old_mpe_threshold(stat) 
            print('  %s  IceTop PMT thresh = %s V !!' % (str(omkey), thresh/I3Units.V))
            success = False

    # FrontEndImpedance
    fei = cal.front_end_impedance
    if isnan(fei) \
           or fei < 30 * I3Units.ohm or fei > 50 * I3Units.ohm :
        print('  %s  front end impedance = %s ohms !!' % (str(omkey), fei/I3Units.ohms))
        success = False

    # SPEMean
    spemean = dc.spe_mean(stat, cal) 
    if isnan(spemean) or not spemean > 0.0 :
        print('  %s  SPE Mean = %s !!' % (str(omkey), dc.spe_mean(stat, cal) ))
        success = False

    # TransitTime
    transittime = dc.transit_time(stat, cal)
    if isnan(transittime) :
        print('  %s  transit time = %s ns !!' % (str(omkey), transittime/I3Units.ns))
        success = False

    # ATWD Gains
        
    # ATWD Beacon Baslines

    # ATWD DeltaTs

    # ATWD BinCalibFit

    # FADC Gain

    # FADC Beacon Basline

    # FADC DeltaT

    # LC Window Pre

    # LC Window Post

    # ATWD Status A

    # ATWD Status B

    # LC Mode

    # TX Mode

    # LCSpan

    # DOM Gain Type

    # ATWDSamplingRate
    
    # DiscriminatorPulse template
    
        
    return success

    
