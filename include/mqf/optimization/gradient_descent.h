#ifndef INCLUDED_MQF_OPTIMIZATION_GRADIENT_DESCENT
#define INCLUDED_MQF_OPTIMIZATION_GRADIENT_DESCENT
#include "linesearch.h"

namespace mqf {
	
	template<typename Geodesic>
	struct GradientDescent {
		using Metric = typename Geodesic::Metric;
		using Point = typename Metric::Point;

		Metric metric;
		LineSearch lineSearch;
		uint32_t maxSteps = 1000;

		Point x;
		
		template<typename S,typename DS>
		bool step( const S& cost, const DS& gradient ) {

			Geodesic geodesic( x, -gradient(x) );

			double alpha = lineSearch.search(
					[&]( double t ){
					return cost( geodesic(t) );
				},
				[&]( double t ){
					Point xt = geodesic(t);
					return metric(xt)(
						gradient(xt),
						geodesic.parallelTranslate( geodesic.velocity, t )
					);
				}
			);

			if( alpha <= 0.0 ) return false;

			x = geodesic(alpha);

			return true;
		}

		template<typename S,typename DS>
		Point optimize( const Point& initial, const S& cost, const DS& gradient ) {
			x = initial;
			lineSearch.reset();
			for(uint32_t i=0;i<maxSteps;++i) {
				if( !step( cost, gradient ) )
					break;
			}
			return x;
		}

	};
}

#endif
