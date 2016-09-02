#ifndef I3_SHOVEL_RENDER_COLOR_VARIANT_H
#define I3_SHOVEL_RENDER_COLOR_VARIANT_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <QPixmap>

#include "SceneVariant.h"


class ColorMap;
typedef boost::shared_ptr<ColorMap> ColorMapPtr;

/**
 * Base class of color map types, plus some static utility methods for dealing with
 * collections of color maps
 */
class ColorMap{
public:
	struct NamedMap{
		std::string name;
		ColorMapPtr map;
		NamedMap( const std::string& n, ColorMapPtr p ):
			name(n), map(p)
		{}
	};
	typedef std::vector< NamedMap > mapList;

private:
	static mapList python_maps_;

public:
	virtual ~ColorMap(){}
	virtual QColor value( float v ) = 0;
	virtual QPixmap image( int x = 128, int y = 1 );
	virtual QPixmap image( QSize qs ){
		return image( qs.width(), qs.height() );
	}

	/**
	 * @return A list of "steamshovel-native" color maps defined in pure C++
	 * The return value is given by value.  The caller will receive unique ColorMapPtrs
	 * in the returned list
	 */
	static mapList availableNativeMaps();

	/**
	 * @return a constant list of color maps generated from python.
	 */
	static const mapList& availablePythonMaps();

	/**
	 * Get a string representation of a color map.  These strings will be executable python constructors
	 * for shovelart pybindings, but they're reasonably human-readable as well.
	 */
	static std::string toString( ColorMapPtr );

	/**
	 * Look up a particular python color map by its name
	 */
	static ColorMapPtr getPythonMapByName( const std::string& str );

};

/** Color map with a constant alpha value */
class RgbColorMap : public ColorMap{
protected:
	float alpha_;

public:
	RgbColorMap() : ColorMap(), alpha_(1.f)
	{}

	float alpha() const { return alpha_; }
	void setAlpha( float a ){ alpha_ = a; }

	virtual QColor rgbValue( float v ) = 0;

	virtual QColor value( float t ){
		QColor v = rgbValue(t);
		v.setAlphaF( alpha_ );
		return v;
	}


};

/** The traditional clamped HSV color map adapted from glshovel */
class I3TimeColorMap : public RgbColorMap{

public:
	virtual QColor rgbValue( float v ){
		float hue = v * 0.7f;
		return QColor::fromHsvF( hue, 1, 1 );
	}

};

/** Color map returning a constant value at all times */
class ConstantColorMap : public RgbColorMap{
protected:
	QColor c_;
public:
	ConstantColorMap( const QColor& c, float alpha = 1.0f ):
		RgbColorMap(), c_(c)
	{
		setAlpha(alpha);
	}

	void setColor( QColor c ){
		c_ = c;
	}

	virtual QColor rgbValue( float ){
		return c_;
	}

};

/** A color variant type that generates values from a ColorMap */
class TimeWindowColor : public SceneVariant<QColor> {
private:
	ColorMapPtr map_;
	const TimeWindow* win_;
	double t_;
public:
	TimeWindowColor( const TimeWindow* window, double t, ColorMapPtr cm ) :
		map_(cm), win_(window), t_(t)
	{
		//map_.setAlpha( alpha );
	}

	virtual QColor value( double vistime ){
		float normalized_t = ( t_ - win_->colorStart() ) / ( win_->colorEnd() - win_->colorStart() );
		normalized_t = std::min( 1.f, std::max( 0.f, normalized_t ) );
		return map_->value( normalized_t );
	}
};

I3_POINTER_TYPEDEFS( TimeWindowColor );

#endif /* I3_SHOVEL_RENDER_COLOR_VARIANT_H */
