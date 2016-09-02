#ifndef SPLINEFLUX_H
#define SPLINEFLUX_H

#include <NewNuFlux/NewNuFlux.h>

extern "C" {
#include <photospline/splinetable.h>
#include <photospline/bspline.h>
}

namespace NewNuFlux{
	
	namespace detail{
		class splinetable{
		private:
			::splinetable data;
			int* centerBuffer;
			//disallow copying
			splinetable(const splinetable&);
			splinetable& operator=(const splinetable&);
		public:
			splinetable():centerBuffer(NULL){
				data.ndim=0;
			}
			splinetable(std::string tablefile){
				memset(&data,0,sizeof(data));
				int res=readsplinefitstable(tablefile.c_str(), &data);
				if(res!=0)
					log_fatal_stream("Failed to read spline from " << tablefile);
				assert(data.ndim>0);
				centerBuffer=new int[data.ndim];
				assert(centerBuffer!=NULL);
			}
#if __cplusplus >= 201103L
			splinetable(splinetable&& other){
				memcpy(&data,&other.data,sizeof(data));
				memset(&other.data,0,sizeof(other.data));
				centerBuffer=other.centerBuffer;
				other.centerBuffer=NULL;
			}
#endif
			~splinetable(){
				splinetable_free(&data);
				delete[] centerBuffer;
			}
			size_t getDimension() const{ return(data.ndim); }
			double operator()(const double* x) const{
				if(tablesearchcenters(&data, x, centerBuffer) == 0)
					return(ndsplineeval(&data, x, centerBuffer, 0));
				return(0.0);
			}
		};
	}
	
	class SimpleSplineFlux : public FluxFunction{
	private:
		std::map<particleType,boost::shared_ptr<detail::splinetable> > components;
	public:
		SimpleSplineFlux(const std::string& fluxName);
		static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);
		///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
		virtual double getFlux(particleType type, double energy, double cosZenith) const;
	};
	
} //namespace NewNuFlux

#endif //SPLINEFLUX_H