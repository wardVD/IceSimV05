namespace xppc{
  float xrnd();
  float grnd();

  void start();
  void stop();
  void choose(int);
  void ini(int);
  void fin();
  void eout();

  void flini(int, int);
  void flone(unsigned long long);

  struct mcid:std::pair<int,unsigned long long>{
    int frame;
  };

  struct DOM{
    float r[3];
  };

  struct ikey{
    int str, dom;

    bool operator< (const ikey & rhs) const {
      return str == rhs.str ? dom < rhs.dom : str < rhs.str;
    }
  };

  struct OM:DOM,ikey{};
  extern std::vector<OM> i3oms;
  extern std::map<ikey, float> rdes, hvs;
  void initialize(float);
  const DOM& flget(int str, int dom);
  void flshift(float [], float []);

  struct ihit{
    ikey omkey;
    mcid track;
    float time;
#ifdef PDIR
    float dir;
#endif
  };

  extern std::vector<ihit> hitz;

  void eini();
  void efin();

  void sett(float, float, float, std::pair<int,unsigned long long>, int);
  template <class T> void addp(float, float, float, float, float, T);
}
