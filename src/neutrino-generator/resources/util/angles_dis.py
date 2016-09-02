'''
Created on 10 August 2013

@author: GAZ
'''
import numpy as np
import pylab as plt

#==================================================
class lept:
    def __init__(self,lname):  # set according to the name of lepton
        if   lname == 'e'   : self.m = 5.10998928E-4  # $m_e$
        elif lname == 'mu'  : self.m = 0.1056583715   # $m_\mu$
        elif lname == 'tau' : self.m = 1.77628        # $m_\tau$
        else :             print ' Wrong value of lepton '
        self.m2  = self.m**2
        self.M   = 0.939                # mass of nucleon in GeV
        self.M2  = self.M**2  
        self.mpi = 0.1396               # mass of pion
        self.Wmin= self.M + 2*self.mpi  # minimum mass of hadrons in DIS     
#--------
# minimum allowed value of x for given E and lepton
    def Xmin(self,E):
            return self.m2/(2*self.M*(E-self.m))
#--------
# upper branch of Y(x) due to mass of lepton
    def A(self,E,x):
        return (1.0 - self.m2*(1.0/(2.0*self.M*E*x) + 0.5/E/E))/(2 + self.M*x/E)
#--------
    def B(self,E,x):
        s1 = 1.0 - self.m2/(2.0*self.M*E*x) - self.m/E
        if s1 < 0. : s1 = 0.0
        s2 = 1.0 - self.m2/(2.0*self.M*E*x) + self.m/E
        return np.sqrt(s1)*np.sqrt(s2)/(2.0 + self.M*x/E)
#--------
    def YLim(self,E,x):
        a = self.A(E,x)
        b = self.B(E,x)
        yl = a - b
        yu = a + b
        return (yl,yu)
#--------
# limit from W_min^2
    def YW2(self,E,x,W2):
        return (W2 - self.M2)/(2*self.M*E*(1.0 - x))

#--------
    def GetTheta(self,E,x,y):
        y1  = 1 - y
        xmn = self.Xmin(E)
        if x < xmn: 
            print ' x < x_min=',xmn, '  stop!'
            raw_input()
        (yl,yu) = self.YLim(E,x)
        if (y-yl)*(y-yu) > 0 :
            print ' y=',y,' is to be in [',yl,',',yu,']  stop!'
            raw_input()
        kappa = (self.m/E)**2
        beta_l = np.sqrt(1 -kappa/y1**2)
        cos_theta = (1 - (self.M*x*y/E + kappa/2)/y1)/beta_l
        theta = np.arccos(cos_theta)/np.pi*180
        print 'cos_theta=%6.4E, theta in degrees=%.3f' % (cos_theta,theta)        
        return cos_theta 
#--------
# W is the mass of the produced inclusive hadron state
    def PlotAllowed(self,E,W):
        W2 = W*W
        if W < self.Wmin : 
            raw_input(' W is too small for DIS,  stop!')
        if E <= ((self.m + W)**2 - self.M2)/(2*self.M) :
            raw_input(' energy is below the threshold of DIS,  stop!') 
        npt = 501 # number of points
        xmn = self.Xmin(E)
        xx = np.zeros(npt,float)
        yyl = np.zeros(npt,float)
        yyu = np.zeros(npt,float)
        yw2 = np.zeros(npt,float)
        ylw = np.zeros(npt,float)       
        for i in range(npt):
            xx[i] = xmn + (1.0 - xmn)*((1.*i)/(npt-1))**3
            xx[npt-1] = 1.0 - 1E-7     # to avoid 1-x = 0 at x=1
            (yyl[i],yyu[i]) = self.YLim(E,xx[i])
            yw2[i] = self.YW2(E,xx[i],W2)
            ylw[i] = yyl[i]
            if yw2[i] > yyl[i] : ylw[i] = yw2[i] 
            
        plt.plot(xx,yyl,'b-')
        plt.plot(xx,yyu,'b-')
        plt.plot(xx,yw2,'g-')
        plt.fill_between(xx, ylw, yyu,where=yyu>ylw,color='y',alpha=0.5,hatch='//')
        plt.xscale('log')
        plt.yscale('log')
        plt.xlim(0.3*xmn,1)
        plt.ylim(yyl[npt-1]/2,2)
        plt.xlabel('x')
        plt.ylabel('y(E,x)')
        plt.grid(True)
        plt.minorticks_on()
        plt.show()
        
# End of class lepton

''' ============================== '''
# You may choose type of lepton, energy and 
# minimum mass of invariant state                
l = lept('tau')
E = 10.
Wmin = 1.4
#l.PlotAllowed(E,Wmin)
#print 'Plot done'

# Get the cosine of $\theta$ ant angle in degrees
#while True :
for i in range(10):
    x = float(raw_input('x='))
    y = float(raw_input('y='))
    print ' x=',x, ' y=',y
    l.GetTheta(E,x,y)
    
print "end"

    
