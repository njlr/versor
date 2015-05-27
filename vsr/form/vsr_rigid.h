/*
 * =====================================================================================
 *
 *       Filename:  vsr_rigid.h
 *
 *    Description:  rigid body constraints using pointer networks
 *
 *        Version:  1.0
 *        Created:  07/10/2014 15:53:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  vsr_rigid_INC
#define  vsr_rigid_INC

namespace vsr { namespace cga {

struct Constrain {

   //unused?
    static Point Crease (const Pnt& a, const Pnt& dls, const Dll d, bool mtn){
      // Dls dls = round::at(b, a);
       Circle cir = a ^ d;
       Pair par = ( dls ^ cir.dual()).dual();
       return round::loc( round::split(par,mtn) );
    }


    /* static Pair Triple(const Dls& da, const Dls& db, const Dls & dc){ */
    /*   return (da ^ db ^ dc).dual(); */
    /* } */

    // three distances, counter clockwise (deprecated, use Tetral)
    static Point Triple (const Dls& da, const Dls& db, const Dls& dc, bool mtn){
       return round::loc( round::split( (da ^ db ^ dc).dual(), mtn ) ) ; 
    }
    // tetral constraint
    static Point Tetral (const Dls& da, const Dls& db, const Dls& dc, bool mtn){
       return round::loc( round::split( (da ^ db ^ dc).dual(), mtn ) ) ; 
    }
    //planar constraint: two distances and a plane
    static Point Planar( const Dls& da, const Dls& db, const Dls& dc, bool mtn){
       Plane plane = da ^ db ^ dc ^ Inf(1);
       return round::loc( round::split( (da ^ dc ^ plane.dual() ).dual(), mtn ) );
    }

    //Circle tangency constraint, two distances and an original point (closest to original)
    static Point Tangency(const Pnt& p, const Dls& da, const Dls& db){

      auto meet = (da ^ db).dual();
      auto tan =  round::loc( tangent::at( meet, p ) );
      auto sur = round::sur( meet );
      auto line = tan ^ sur ^ Inf(1);
      auto np = round::split ( ( line.dual() ^ sur).dual(), false );

      return np;
      
    }

    //Spherical tangency constrain, one distance and an original point (returns point on sphere closest to p);
     static Point Tangency(const Pnt& p, const Dls& da){
       auto line =  p ^ da ^ Inf(1);
       auto np = round::split( (line.dual() ^ da).dual(), false);
       return np;
     }

    //Tension tangency constrain, one distance and an original point 
    //(returns point on sphere closest to p if p is outside da, otherwise just p);
     static Point Tension(const Pnt& p, const Dls& da){
       if ( (p<=da)[0] > 0 ) return p;
       auto line =  p ^ da ^ Inf(1);
       auto np = round::split( (line.dual() ^ da).dual(), false);
       return np;
     }

    //tension constraint from two distances
    static Point Tension(const Pnt& p, const Dls& da, const Dls& db){
       
       if ( ( (p<=da)[0] > 0 ) && ( (p<=db)[0] > 0 ) ) return p; // returns input point if it lies within both spheres 
       
       //otherwise, constrain to lie on circle meet
       auto meet = (da ^ db).dual();
       auto tan =  round::loc( tangent::at( meet, p ) );
       auto sur = round::sur( meet );
       auto line = tan ^ sur ^ Inf(1);
       auto np = round::split ( ( line.dual() ^ sur).dual(), false );

       return np;     
    }



    // three distances, center of meet pre-SPLIT (experimental)
    static Point Triple0 (const Dls& da, const Dls& db, const Dls& dc){
       return round::loc( (da ^ db ^ dc).dual()  ) ; 
    }


    /* static Point Fabrik(const Dls& base, const Dls& goal){ */

    /*       //repeat until distance is decreased to within error threshold, or give up after 20 iterations */
    /*       while (s[0] > err){ */
              
    /*           Pnt tmpGoal = goal; */
    /*           Pnt tmpBase = base; */
              
    /*           static Dls dls; //surround */
    /*           static Dll dll; //line */
    /*           static Par par; //intersection of line ^ surround */
              
    /*           //backward reaching */
    /*           for (int i = end; i > begin; --i){ */
    /*               mFrame[i].pos( tmpGoal );           //set position of ith frame */
    /*               dls = prevDls(i);                   //set boundary sphere through i-1 th frame; */
    /*               dll = linb(i);//.sp( !mLink[i].mot() );                      //get line from ith to i-1th frame */
    /*               par = (dll ^ dls).dual();           //get point pair intersection of line and boundary sphere */
    /*               tmpGoal = round::split(par,true);      //extract point from point pair intersection */
    /*           } */
              
    /*           //forward correction */
    /*           for (int i = begin; i < end; ++i){ */
    /*               dls = nextDls(i);                   //set boundary sphere through i+1 th frame */
    /*               dll = linf(i);                      //get line to i+1th frame; */
    /*               par = (dll ^ dls).dual();           //get point pair intersection of line and boundary sphere */
    /*               tmpBase = round::split(par,true); */
    /*               mFrame[i+1].pos(tmpBase);           //set position of i+1th frame */
    /*           } */
              
    /*           s = mFrame[ end ].pos() <= p * -2.0; */
               
    /*           n++;  if (n > 20) {  break; } */
    /*       } */

    /* } */

  
};

// Holds a pointer to a source, and has a radius t, 
// operator() generates a dual sphere at source with radius t
struct DistancePtr {
  Point * src; // center 
  float t;

  DistancePtr(){};

  DistancePtr( Pnt& a, const Pnt& target)  
  {
    set(a,target);  
  }

  void set(Pnt& a, const Pnt& target){
    src = &a; t = round::rad( round::at(*src,target) );
  }

  Dls operator()(){ return round::dls( *src, t ); }

};

struct Rigid3{

  DistancePtr da,db,dc;

  void set(const Pnt& target, Rigid3 * ra, Rigid3 * rb, Rigid3 * rc){

  }
};


/// A Rigid Constraint Node set by Three Distance Pointers
struct Rigid{
  //default calc is false until ra parents are set
  bool bCalc, bTriple;
  Pnt result;
  //mountain or valley
  bool mtn;

  //Has three distances
  DistancePtr da,db,dc;
  
  //Is Dependent on three parents
  Rigid *ra, *rb, *rc;

  //Has n children which depend on it
  vector<Rigid*> child;


  Rigid() : bCalc(false), bTriple(true), ra(NULL), rb(NULL), rc(NULL) {}

  Rigid(const Pnt& res ) : bTriple(true),  ra(NULL), rb(NULL), rc(NULL) {
    set(res);
  }
  void set(const Pnt& res){
    bCalc = false;
    result = res;
  }

  Rigid( const Pnt& target,  Rigid * pa,  Rigid * pb,  Rigid * pc, bool m) : bTriple(true) 
  {
    set(target,pa,pb,pc,m);
  }

  //Counter Clockwise
  void set( const Pnt& target, Rigid * pa, Rigid * pb, Rigid * pc, bool m){
    mtn = m; bCalc=true;
    ra = pa; rb = pb; rc = pc;
    result = target; 
    da.set(ra->result,target);
    db.set(rb->result,target);
    dc.set(rc->result,target); 
  }

  //Counter Clockwise
  void set( Rigid * pa, Rigid * pb, Rigid * pc, bool m){
    mtn = m; bCalc=true;
    ra = pa; rb = pb; rc = pc;
    da.set(ra->result,result);
    db.set(rb->result,result);
    dc.set(rc->result,result); 

    //add this to children of three others
    ra->child.push_back(this); rb->child.push_back(this); rc->child.push_back(this);
  }


  //hmmm?
  void set( Rigid * pa, Rigid * pb, bool m){
    mtn =m; bCalc=true; bTriple=false;
    ra=pa; rb=pb;rc=this;
    da.set(ra->result,result);
    db.set(rb->result,result);
    dc.set(rc->result,result);

    //add this to children of three others
    ra->child.push_back(this); rb->child.push_back(this); rc->child.push_back(this);
  }

  void reset(){ 
    if (ra!=NULL && rb!=NULL && rc!=NULL) bCalc = true; 
  }

  Circle circleA(){
    return (da()^db()).dual();
  }
  Circle circleB(){
    return (db()^dc()).dual();
  }  
  void orbitA(float amt){
    result = round::pnt_cir( circleA(), amt * ( mtn?1:-1) ); 
  }
  void orbitB(float amt){
    result = round::pnt_cir( circleB(), amt * ( mtn?1:-1) ); 
  } 


  Pnt up(){
    if (bCalc) {
      bCalc=false;                             // lock in case network graph is looped
      for (auto& i : child) i -> down();       // cascade children
      (*ra).up(); (*rb).up(); (*rc).up();
      // satisfy();
      result = bTriple ? Constrain::Triple(da(),db(),dc(),mtn) : Constrain::Planar(da(),db(),dc(),mtn);
    }
    return result;
  }

  //calculate current position based on parents
  void update(){
    if (bCalc){
      result = bTriple ? Constrain::Triple(da(),db(),dc(),mtn) : Constrain::Planar(da(),db(),dc(),mtn);
    }
  }

  //broadcast position to child
  void down(){
      if (bCalc){
        bCalc=false;
        for (auto& i : child) {
          i -> update();
         // i -> satisfy();
          i -> down();
        }             
     //   up();  
      } 
  }

  Pair meet(){
    return ( da() ^ db() ^ dc() ).dual();
  }

  //bring three spheres closer together towards mutual center until meet is legit.
  void satisfy(int max=20){
    auto& pa = *da.src; auto& pb = *db.src; auto& pc = *dc.src;
    auto rs = round::size(meet(),false);
    int iter=0;
    while ( rs < -.0001 && iter < max ){
      auto center = round::loc(pa^pb^pc);
      pa = round::null( pa+(Vec(center-pa)*fabs(rs)) );
      pb = round::null( pb+(Vec(center-pb)*fabs(rs)) );
      pc = round::null( pc+(Vec(center-pc)*fabs(rs)) );
      rs = round::size( meet(), false );
      iter++;
    }
  }
};





struct Rigid2{
  
  /// Meet of parents
  Point result;

  /// iteration
  int iter=0;

  /*-----------------------------------------------------------------------------
   *  Parents Generate Circles to Control This Result
   *-----------------------------------------------------------------------------*/
  struct Parents{
    DistancePtr da,db;
    Rigid2 *ra, *rb;          //parents

    Circle meet(){
      return (da()^db()).dual();
    }

    bool inside(const Point& p){
      return ( (p<=da())[0] > 0 ) && ( (p<=db())[0] > 0 );
    }

    void operator()(){
      (*ra)(); (*rb)();
    }
  };

  bool bMtn,bCalc,bReCalc;

  /// possibly many couples (one per valence)
  vector<Parents> parents;
  vector<Rigid2*> child;    //children


  Rigid2() : bCalc(false), bReCalc(false) {}
  Rigid2(const Pnt& res) { set(res); }

  void set(const Pnt& res){
    bCalc=false; bReCalc=false;
    result=res;
  }

  //add parents
  void add(Rigid2 *pa, Rigid2 *pb, bool m){
    
    bCalc=true; bReCalc=true;
    
    //make new parents
    Parents p;
    p.ra=pa; p.rb=pb;
    
    //set distance contraints based on pa and pb
    p.da.set(pa->result,result);
    p.db.set(pb->result,result);

    //add this to list of children of just pa 
    pa->child.push_back(this);
   // pb->child.push_back(this);
    parents.push_back(p); 
  }


  /* Rigid2(const Pnt& target, Rigid2* pa, Rigid2*pb, bool m) */
  /* : result(target), ra(pa), rb(pb), bCalc(true), bMtn(m) */
  /* { */
  /*   da.set(ra->result,target); */
  /*   db.set(rb->result,target); */

  /*   ra->child.push_back(this); rb->child.push_back(this); */
  /* } */
 
  void reset(){ bCalc=true; bReCalc=true; iter=0; }

  void operator()(){
    if (bCalc){
     // Pnt np = result;
      bCalc=false;
      for(auto& i : parents){
        i();
        //np = Constrain::Tangency(np, i.da(), i.db());
      }
      update();
      //result = np;
    }
    //return result;
  }

  void update(){
    if(bReCalc){
      for(auto& i : parents){
        result = Constrain::Tangency(result, i.da(), i.db());
      }
    }
  }

  //update only relative to parents with r
  void satisfy_old(Rigid2 * r){
    if(bReCalc){
      bReCalc=false;
      bool bRepeat = true;
      int iter=0;
      while(bRepeat && (iter<10) ){
        bRepeat=false;
        iter++;
        for(auto& i : parents){
          if ( (r==i.ra) || (r==i.rb) ){
            bool bCocircular = ( fabs( (result <= i.meet()).wt() ) < .001);
           // bool bInside = i.inside(result);
            if (!bCocircular) {
              result = Constrain::Tangency(result, i.da(), i.db());
              bRepeat = true;
             }           
           }
          }
        }
      }
  }

  bool hasA(Rigid2 * r){
    for (auto& i : parents){
      if (r==i.ra) return true;
    }
    return false;
  }

  bool hasB(Rigid2 * r){
    for (auto& i : parents){
      if (r==i.rb) return true;
    }
    return false;
  }



  void satisfy_forward(Rigid2 * r){
    if (bReCalc){
      for(auto& i : parents){
        if (r==i.ra){
            result = Constrain::Tangency(result, i.da(), i.db());
         }
      }
    }
  }
  void satisfy_backward(Rigid2 * r){
     if (bReCalc){
      for(auto& i : parents){
        if (r==i.rb){
            result = Constrain::Tangency(result, i.da(), i.db());
         }
      }
     }
  }

  float error(Rigid2 * r){
    float tf=0; bool bFound=false;
    for (auto& i : parents){
      if (r==i.ra){
         bFound=true;
         tf = fabs((result <= i.meet()).wt());
       }
    }
    if (!bFound){
      for (auto& i : parents){
        if (r==i.rb){
          tf = fabs((result <= i.meet()).wt());
        }
      }
    }
    return tf;

  }


  /* void fabrik(Rigid2 * r){ */
  /*   if (bReCalc){ */
  /*     bool bRepeat = true; */
  /*     int iter =0; */
  /*     while (bRepeat && (iter<100)){ */
  /*       bRepeat=false; */
  /*       iter++; */
  /*       for (auto& i : parents){ */
  /*         if (r==i.ra) */
  /*       } */
  /*     } */
  /*   } */
  /* } */

  //set bReCalc to true
  bool checkRecalc(){
    for (auto& i : parents){
      if (i.ra->error(this)>.001){
        bCalc=true;
        bReCalc=true;
        i.ra->bReCalc=true;
        i.ra->bCalc=true;
      }
    }
    return bReCalc;
  }

  vector<Rigid2*> satisfy(int begin, int end){
    
      if (!parents.empty()){
        int tIter=0;
        float tf;
        //find first not already locked
        Rigid2 * tr;
        for (auto& i : parents){
          if (i.ra->bReCalc){
            tr = i.ra;
            break; 
          }
        }
        //cout << parents[0].ra->hasA(this) << endl;
        bool bLoop = parents[0].ra->hasA(this);
        if (bLoop){
            do{
              tIter++;         
              for (int i=begin; i<=end;++i){
                parents[i].ra->satisfy_forward(this);
              }
              for(int i =end; i >= begin; i-- ) {
                parents[i].ra->satisfy_backward(this);
              }
              tf = tr->error(this);//parents[begin].ra->error(this);
            }while( (tf>.001) && (tIter < 10));
        } else {
            do{
              tIter++;         
              for (int i=begin; i<=end;++i){
                parents[i].ra->satisfy_forward(this); 
                parents[i].rb->satisfy_forward(this); //only last one
              }
              for(int i =end; i >= begin; i-- ) {
                parents[i].rb->satisfy_backward(this);
                parents[i].ra->satisfy_backward(this); //only first one
              }
              //this should test for first non-already bound ...
              tf = tr->error(this);//parents[begin].ra->error(this);
            }while( (tf>.001) && (tIter < 10));
            //cout << "iter " << tIter << endl;
        }
      }
      
       vector<Rigid2*> temp;

       //mark parents as satisfied
       for (auto& i : parents){
         if(i.ra->bReCalc==true) temp.push_back(i.ra);
         i.ra->bReCalc=false;
        // i.rb->bReCalc=false;
       }
       bool bLoop = parents[0].ra->hasA(this);
       if (!bLoop){
         if(parents.back().rb->bReCalc==true) temp.push_back(parents.back().rb);
         parents.back().rb->bReCalc=false;
       }

       return temp;
  }

  
  // go through all parents satisfy relationship to this
  void cascade(int begin, int end){
   
      //do this forward and backward reach how many times?
      //until first node is within error of first forward meet
      auto rp = satisfy(begin,end);
      cout << "parents to solve: " <<  rp.size() << endl;
      while (!rp.empty()){
        //cascade connections 
        vector<Rigid2*> temp;
        for (auto& i : rp){
            auto r = i->satisfy(0, i->parents.size()-1);
            for (auto& j : r){
              temp.push_back(j);
            }
            //i.ra->cascade(0,i.ra->parents.size()-1);
         }
         //cout << temp.size() << endl;
         rp = temp;
      }

  }

  Cir circle(int idx =0) { return ( parents[idx].da() ^  parents[idx].db() ).dual(); }

  /// get point at theta t around constraint orbit
  Pnt orbit(VSR_PRECISION t, int idx=0) { return round::pnt_cir( circle(idx), t * ( bMtn?1:-1) ); }
};



/*!
 *  \brief  Rig has n spherical constraints to satisfy (try using fabrik solver here)
 */
struct Rig {

    Point result; ///< result of constraint computation
    bool bCalc,bReCalc;
    int iter=0;

    void set(const Pnt& res){
      bCalc=false; bReCalc=false;
      result=res;
    }

    void reset(){
      bCalc=true; bReCalc=true; iter=0;
    }

    struct Parent{
      DistancePtr da;
      Rig * rig;
      bool bStrut =true;

      //set distance constraint to rig r
      void set(const Point& p){
        da.set(rig->result, p);
      }

      //constrain a point p to spherical distance via strut or cable
      Point constrain(const Point& p){//, bool strut){
        return bStrut ? Constrain::Tangency(p,da()) : Constrain::Tension(p, da()); 
      }

      float distance(const Point& p){
        return (p<=da())[0];
      }
    };

    vector<Parent> parent;
    vector<Rig*> child;

    //add a constraint
    void add(Rig * r, bool bStrut=true){
       Parent p;
       p.bStrut = bStrut;
       p.rig = r;
       p.set(result);
       parent.push_back(p);
       p.rig->child.push_back(this);
    }

    void modify(Rig * r, bool bStrut){
      for (auto& i : parent){
        if (r==i.rig) i.bStrut=bStrut;
      }
    }

    //satisfy constraint to rig r
    void satisfy(Rig * r){
      if (bReCalc){       
        for (auto& i : parent){
          bool tCalc=false;
          //if parent is r
          if (r==i.rig) tCalc=true;
          //...or is connected to r
          for (auto& j : i.rig->parent){
            if (r==j.rig) tCalc=true;
          }
          //..calc
          if (tCalc){
            result = i.constrain(result);
          }
        }
      }
    }

    void cascade(){
      if (bCalc){
        iter++;
        if (iter>1000) {
          bCalc=false;  
        }   
        for (auto& i : child){
          i->satisfy(this);
        }
        for (auto& i : child){
        //  i->cascade();
        }
      }
    }

};

/* ----------------------------------------------------------------------------- */
/*  *  SIMPLICIAL CONSTRAINT . . . (n+1)-simplices to constrain (n)-chains connected by (n-1)-simplex edges */ 
/*  *-----------------------------------------------------------------------------*/ 
/* struct Rigid_{ */
/*   bool bCalc, bTriple; */
/*   Pnt result; */

/*   DistancePtr da,db,dc; */
/*   Rigid_ *ra, *rb, *rc; */
/*   bool mtn; */

/*   Rigid_() : bCalc(false), bTriple(true), ra(NULL), rb(NULL), rc(NULL) {} */

/*   Rigid_(const Pnt& res ) : bTriple(true),  ra(NULL), rb(NULL), rc(NULL) { */
/*     set(res); */
/*   } */
/*   void set(const Pnt& res){ */
/*     bCalc = false; */
/*     result = res; */
/*   } */

/*   Rigid_( const Pnt& target,  Rigid * pa,  Rigid * pb,  Rigid * pc, bool m) : bTriple(true) */ 
/*   { */
/*     set(target,pa,pb,pc,m); */
/*   } */

/*   //Counter Clockwise */
/*   void set( const Pnt& target, Rigid * pa, Rigid * pb, Rigid * pc, bool m){ */
/*     mtn = m; bCalc=true; */
/*     ra = pa; rb = pb; rc = pc; */
/*     result = target; */ 
/*     da.set(ra->result,target); */
/*     db.set(rb->result,target); */
/*     dc.set(rc->result,target); */ 
/*   } */

/*   //Counter Clockwise */
/*   void set( Rigid * ra, Rigid * rb, Rigid * rc, bool m){ */
    
/*     mtn = m; bCalc=true; */
/*     da.set(ra->result,result); */
/*     db.set(rb->result,result); */
/*     dc.set(rc->result,result); */ 

/*     //fasten cycle */
/*     ra->set(rb, rc, this, m); */
/*     rb->set(rc, this, ra, m); */
/*     rc->set(this, ra, rb, m); */
    
/*   } */

/*   /1* void set( Rigid * pa, Rigid * pb, bool m){ *1/ */
/*   /1*   mtn =m; bCalc=true; bTriple=false; *1/ */
/*   /1*   ra=pa; rb=pb;rc=this; *1/ */
/*   /1*   da.set(ra->result,result); *1/ */
/*   /1*   db.set(rb->result,result); *1/ */
/*   /1*   dc.set(rc->result,result); *1/ */
/*   /1* } *1/ */

/*   void reset(){ */ 
/*     if (ra!=NULL && rb!=NULL && rc!=NULL) bCalc = true; */ 
/*   } */

/*   Pnt operator()(){ */
/*     if (bCalc) { */
/*       bCalc=false; //lock because network graph is looped */
/*       (*ra)(); (*rb)(); (*rc)(); */
/*       result = satisfy(); //satisfying requires unlocking */
/*     } */
/*     return result; */
/*   } */

/*   Pair meet(){ */
/*     return Constrain::Triple( da(), db(), dc() ); */
/*   } */

/*   //bring three spheres closer together towards center of meet until meet is legit. */
/*   Pnt satisfy(){ */
/*     auto meet = Constrain::Triple( da(), db(), dc() ); */
/*     if ( round::size(meet,false) < -.0001 ){ */
      
/*     } */
/*      return bTriple ? Constrain::Triple(da(),db(),dc(),mtn) : Constrain::Planar(da(),db(),dc(),mtn); */

/*   } */
/* }; */


} } //vsr::cga:;
#endif   /* ----- #ifndef vsr_rigid_INC  ----- */
