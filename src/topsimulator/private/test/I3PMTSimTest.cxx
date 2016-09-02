#include <I3Test.h>

#include <pmt-simulator/pmt/I3PMT.h>
#include <pmt-simulator/I3PMTConstants.h>
#include <phys-services/I3GSLRandomService.h>
#include <sim-services/I3SumGenerator.h>
#include <icetray/I3Units.h>

using namespace std;

TEST_GROUP(PMTSIM);


TEST(mean_npe)
{
    I3GSLRandomServicePtr random(new I3GSLRandomService);
    boost::shared_ptr<I3SumGenerator> sumgen(new I3SumGenerator(random,&I3PMT::PEDistribution,0,3,1000,12));
    I3PMT pmt(random,sumgen);
    
    // Generate hit series
    
    for(double hits=80; hits<10000; hits+=10)
    {
	int num = 1000;
	double sumQ    = 0;
	//double sumQSqr = 0;
	for(int i=0; i<num; i++)
	{
	    double q  = pmt.CalculateCharge(static_cast<unsigned int>(hits/I3PMTConstants::MEAN_NORMALIZED_PE));
	    sumQ    += q;
	    //sumQSqr += q*q;
	}
	
	double mean_npe  = sumQ/num;
	//double sigma_npe = sqrt(sumQSqr/num - mean_npe*mean_npe);
	//double sigma_mean = sigma_npe/sqrt(num);

	double tolerance = 0.01*mean_npe;
	
	cout << "Expected npe = "    << hits << ", "
	     << "output mean_npe = " << mean_npe << ", "
	     << "npe_min = "         << mean_npe - tolerance << ", "
	     << "npe_max = "         << mean_npe + tolerance << endl;
	
	ENSURE_DISTANCE(hits, mean_npe, tolerance, "Generated NPE deviates more than 1.0 % from expectation.");	
    }
}
