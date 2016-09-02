from icecube.shovelart import *
from math import sqrt

class CylinderTest( PyArtist ):

    numRequiredKeys = 0

    def create( self, frame, output ):

        output.addCylinder( vec3d(0,0,0), vec3d( 0, 0, 100), 5, 25 )
        output.addCylinder( vec3d(0,0,100), vec3d( 100,100, 0), 25, 0 )
        output.addCylinder( vec3d(100,100,100), vec3d( -100,-100, -100), 0, 25 )

        for v in [ vec3d(*t) for t in ((0,0,-100), (100,0,0), (-100,0,0), (0,100,0), (0,-100,0)) ]:
            c = output.addCylinder( vec3d(0,0,0 ), v, 5 )
            c.setColor( PyQColor(128,255,128) )

        points = Vec3dList()
        points.append( vec3d(0,0,0) )
        points.append( vec3d(0,0,100) )
        points.append( vec3d(100,100,100) )
        points.append( points[0] )
        output.addStaticLines( points )


        h1 = output.addSphere( 200, vec3d(400, 0, 500) )
        h1.setColor( (.9, .9, .9) )

        h2 = output.addSphere( 25, vec3d( 400, 220, 500 ) )
        h2.setColor( (.1,.1,.1) )

        h3 = output.addSphere( 25, vec3d( 400, -220, 500 ) )
        h3.setColor( (.1,.1,.1) )

        h4 = output.addCylinder( vec3d(0,0,0), vec3d(0,0,0), 0, 0 )
        h4.setLocation( vec3d(400,0,500) )
        h4.setAxis( ConstantVec3d((vec3d( -500, 0, 0 ))))
        h4.setBaseRadius( 90 )
        h4.setTopRadius( ConstantFloat(1) )
        h4.setColor( (.9, .9, .1) )
