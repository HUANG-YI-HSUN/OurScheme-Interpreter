# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <vector>
# include <string.h>
# include <string>
# include <math.h>

using namespace std ;

struct Node {
  string content ;
  Node *left, *right, *parent ; 
};

typedef Node *Nnode ;

struct ForDefineSymbol {
  string name ;
  Node *node ;
};

struct ForTree {
  Node *node ;
  int scope ;
  bool combine ; 
};

struct TypeAndLoc {
  string type ;
  int locx, locy ;
};

class OurScheme {
  vector<string> mout, mtokenstring ;
  vector<int> mquoteloc ;
  vector<TypeAndLoc> merrorcheck, mOuterQuote ;
  vector<Nnode> mnodelist, mnotElist ;
  vector<ForDefineSymbol> mdefinelist, mfunctionlist, mdefinefunctionlist ;
  vector<ForDefineSymbol> mnotevallist ;
  ForDefineSymbol mtempsymbol, mnoteval ;
  TypeAndLoc mTypeAndLoctemp ;
  string mtoken, mexit, mpre ;
  string mmode, mstringappend ; // To determine which function
  string meqvstr_first, meqvstr_second ; // For function Equ
  string mequalstr_first, mequalstr_second ; // For function Equal
  Node *mtnode ;
  char mread ;
  int mnumofL, mdot, mx ; // mx : column
  int my, mnumofAtom, mbehinddot ; // my : line
  int mquotenum, mstage ;
  int merrorcheck_loc ;
  float mresult ;
  bool mnum, mEOF, mleftmode ; // mnum : whether mtoken is number or not
  bool mdoterror, mError, mover ;
  bool mfirst, mstringM, mquote ; // -------------------------------------------------
  bool mnoclosingquote, mThisLineHasSth, mcomment ; // mcomment : To deal with the (x,y) 
                                                    // problem about comment
  bool mquotefirst, mconvert, mdone ; // mquotefirst : To deal with the pretty print about quote
  bool mpass, mfloat, msymbolerror ;
  bool mquoteerror, mrepeat ; 
  public : 

  void NewNode( Node *anode, string str ) {
    anode -> content = str ;
    anode -> left = NULL ;
    anode -> right = NULL ;
  } // NewNode()

  void NewNode2( Node *anode, string str, Node *leftt, Node *rightt ) {
    anode -> content = str ;
    anode -> left = NULL ;
    anode -> right = NULL ;
    anode -> left = leftt ;
    anode -> right = rightt ;
  } // NewNode2()
  
  void Preorder( Node *a, bool offset, bool lastLP, bool left, int spacenum ) {
    bool haveLP = false, hasquote = false ;
    if ( a != NULL && ( a -> content == "\'"
                        || a -> content == "quote" )
         && left ) {
      if ( !lastLP )
        for ( int i = 0 ; i < spacenum  ; i++ )
          cout << " " ;
    
      if ( offset )
        cout << "quote" << endl ;
      else
        cout << "( quote" << endl ;
    } // if 
    else if ( a != NULL && a -> content != "" && a -> content != "{DEFINE}"  ) {
      if ( !lastLP )
        for ( int i = 0 ; i < spacenum ; i++ )
          cout << " " ;

      cout << a -> content << endl ;
    } // else if
    else if ( a != NULL && a -> content == "" && !offset &&  a -> left != NULL
              && a -> left -> content != "\'" && a -> left -> content != "quote" ) {
      haveLP = true ;
      if ( !lastLP )
        for ( int i = 0 ; i < spacenum ; i++ )
          cout << " " ;
      cout << "( " ;
    } // else if

    if ( a -> left != NULL ) {
      // cout << "left" << endl ;
      if ( !offset && a -> content != "{DEFINE}" &&
           a -> left -> content != "\'" &&
           a -> left -> content != "quote" )
        spacenum = spacenum + 2 ;

      if ( a -> content == "{DEFINE}" && lastLP ) 
        haveLP = true ;

      if ( ( a -> left -> content == "\'" 
             || a -> left -> content == "quote" ) ) {
        hasquote = true ;
        if ( lastLP )
          haveLP = true ;
      } // if

      if ( haveLP )
        Preorder( a -> left, false, true, true, spacenum ) ;
      else 
        Preorder( a -> left, false, false, true, spacenum ) ;
    } // if

    if ( a -> right != NULL ) {
      // cout << "right" << endl ; 
      if ( hasquote ) {
        spacenum = spacenum + 2 ;
        Preorder( a -> right, false, false, false, spacenum ) ;
        spacenum = spacenum - 2 ;
      } // if
      else if ( a -> right -> content == "" &&
                a -> right -> left -> content != "\'" &&
                a -> right -> left -> content != "quote" ) {
        Preorder( a -> right, true, false, false, spacenum ) ;
      } // else if
      else if ( a -> right -> content != "nil" ) {
        for ( int i = 0 ; i < spacenum ; i++ )
          cout << " " ;

        cout << "." << endl ;
        Preorder( a -> right, true, false, false, spacenum ) ;
      } // else if
    } // if

    if ( !hasquote )
      spacenum = spacenum - 2 ;

    if ( ( haveLP && a -> content != "{DEFINE}" ) || hasquote ) {
      for ( int i = 0 ; i < spacenum ; i++ )
        cout << " " ;
      cout << ")" << endl ;
    } // if
  } // Preorder()

  void PreorderSame( Node *a, bool offset, bool lastLP, bool left, vector<string> &record ) {
    bool haveLP = false, hasquote = false ;
    if ( a != NULL && ( a -> content == "\'"
                        || a -> content == "quote" )
         && left ) {

      record.push_back( "( quote" ) ;
    } // if 
    else if ( a != NULL && a -> content != "" && a -> content != "{DEFINE}"  ) {

      record.push_back( a -> content ) ;
    } // else if
    else if ( a != NULL && a -> content == "" && !offset && a -> left != NULL
              && a -> left -> content != "\'" && a -> left -> content != "quote" ) {
      haveLP = true ;

      record.push_back( "( " ) ;
    } // else if

    if ( a != NULL && a -> left != NULL ) {
      record.push_back( "left" ) ;

      if ( a -> content == "{DEFINE}" && lastLP ) 
        haveLP = true ;

      if ( ( a -> left -> content == "\'" 
             || a -> left -> content == "quote" ) ) {
        hasquote = true ;
        if ( lastLP )
          haveLP = true ;
      } // if

      if ( haveLP )
        PreorderSame( a -> left, false, true, true, record ) ;
      else 
        PreorderSame( a -> left, false, false, true, record ) ;
    } // if

    if ( a != NULL && a -> right != NULL && 
         a -> right -> content != "nil" ) {
      record.push_back( "right" ) ;
      if ( hasquote )
        PreorderSame( a -> right, false, false, false, record ) ;
      else if ( a -> right -> content == "" ) {
        PreorderSame( a -> right, true, false, false, record ) ;
      } // else if
      else {
        record.push_back( "." ) ;
        PreorderSame( a -> right, true, false, false, record ) ;
      } // else
    } // if

    if ( haveLP || hasquote ) {
      record.push_back( ")" ) ;
    } // if
  } // PreorderSame()

  bool Onlyone( vector<ForTree> a ) { // check for only one atom in ()
    if ( a.size() < 1 )
      return false ;

    int base = a.size() - 1 ;
    int count = 0 ;
    for ( int i = base - 1 ; i >= 0 && ( a.at( i ).scope == a.at( base ).scope || 
                                         ( a.at( base ).scope - a.at( i ).scope < 1
                                           && a.at( i ).combine ) ) ; i-- ) {
      count++ ;
    } // for

    if ( count == 0 )
      return true ;

    return false ;
  } // Onlyone()

  void BuildTree() {
    vector< ForTree > list ;
    ForTree temp ;
    vector< int > dotlist ;
    bool first = true, twonode = false, onenode = false ;
    bool qfirst = true, qquotesingle = false ;
    bool hasquotefront = false, quote = false, qquote = false ;
    int numofl = 0, scopeoflast = 0, quoteRP = 0 ;
    int quoteargument = 0, quotescope = 0 ;
    for ( int i = 0 ; i < mtokenstring.size() ; i++ ) {
      if ( mtokenstring.at( i ) == "(" ) {
        numofl++ ;

        if ( qquote )
          quotescope = numofl ;
      } // if
      else if ( mtokenstring.at( i ) == ")" ) {
        if ( numofl == quotescope ) {
          quote = false ;
          qquote = false ;
        } // if

        if ( list.size() == 1 && mtokenstring.size() == 3 ) {
          Node *h = new Node ;
          NewNode2( h, "", list.at( 0 ).node, NULL ) ;
          temp.node = h ;
          temp.scope = numofl ;
          list.pop_back() ;
          list.push_back( temp ) ;
        } // if
        else if ( Onlyone( list ) ) { // Deal with () has only one atom. eg : (1)  
          // cout << list.at( list.size() - 1 ).node -> content << endl ;
          // cout << list.at( list.size() - 1 ).scope << endl ;
          Node *h = new Node ;
          NewNode2( h, "", list.at( list.size() - 1 ).node, NULL ) ;
          temp.node = h ;
          temp.scope = numofl ; 
          list.pop_back() ;
          list.push_back( temp ) ;
          first = false ;
        } // if

        int i = list.size() - 1 ; 
        while ( i > 0 && ( list.at( i - 1 ).scope == numofl || 
                           ( ( list.at( i - 1 ).node -> content == "\'" ) 
                             && list.at( i - 1 ).scope == list.at( i ).scope - 1 ) ) 
                && list.at( i - 1 ).scope != 0 ) {
          if ( list.at( i - 1 ).node -> content == "\'" && 
               list.at( i - 1 ).scope == list.at( i ).scope - 1  ) {
            hasquotefront = true ;
          } // if

          if ( dotlist.size() != 0 && dotlist.at( dotlist.size() - 1 ) == numofl ) {
            dotlist.pop_back() ;
            Node *a = new Node ;
            if ( list.size() != 0 ) {
              onenode = true ;
              scopeoflast = list.at( i ).scope ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( Onlyone( list ) || list.at( i - 1 ).scope == list.at( i ).scope ) ) {
                Node *ar = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *al = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( a, "", al, ar ) ;
              } // if
              else {
                a = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
            } // if
            // Preorder( a ) ; 
            // cout << "--------------------------a" << endl ;

            i = list.size() - 1 ;
            // cout << list.at( i ).scope << " \\++--SCOPE--++// b" << endl ;
            Node *b = new Node ;            
            if ( list.size() != 0 && scopeoflast - list.at( i ).scope < 2 && !hasquotefront ) {
              twonode = true ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( ( Onlyone( list ) && scopeoflast != list.at( i ).scope ) 
                        || list.at( i - 1 ).scope == list.at( i ).scope ) 
                   && list.at( i ).node -> content != "\'" ) {
                Node *br = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *bl = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( b, "", bl, br ) ;
              } // if
              else {
                b = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
              // Preorder( b ) ; 
            } // if
            // cout << "--------------------------b" << endl ;

            if ( twonode && onenode ) {
              twonode = false ;
              onenode = false ;
              Node *temphead = new Node ;
              NewNode2( temphead, "", b, a ) ;
              temp.node = temphead ;
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // if
            else if ( onenode ) {
              onenode = false ;
              temp.node = a ; 
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // else if

            first = false ;
          } // if
          else {
            if ( first ) {
              first = false ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0 ) {
                Node *b = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                while ( i >= 0 && list.at( i ).node -> content == "\'" && list.at( i ).scope == numofl ) {
                  Node *bl = list.at( i ).node ;
                  list.pop_back() ;
                  i-- ; 
                  Node *a = b ;
                  b = new Node ;
                  NewNode2( b, "", bl, a ) ;
                } // while

                Node *h = new Node ;
                NewNode2( h, "", b, NULL ) ;
                temp.node = h ;
                temp.scope = numofl ;
                list.push_back( temp ) ;
                // cout << "quote first" << endl ;
              } // if
              else {
                Node *h = new Node ;
                NewNode2( h, "", list.at( i ).node, NULL ) ;
                temp.node = h ;
                temp.scope = numofl ;
                list.pop_back() ;
                list.push_back( temp ) ;
                // cout << "aaaa" << list.size() << endl ;
              } // else
            } // if
            // Preorder( list.at( i ).node ) ;

            i = list.size() - 1 ;
            // cout << list.at( i ).scope << " \\++--SCOPE--++// a" << endl ;
            Node *a = new Node ;
            if ( list.size() != 0 ) {
              onenode = true ;
              scopeoflast = list.at( i ).scope ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( Onlyone( list ) || list.at( i - 1 ).scope == list.at( i ).scope ) ) {
                Node *ar = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *al = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( a, "", al, ar ) ;
              } // if
              else {
                a = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
            } // if
            // Preorder( a ) ; 
            // cout << "--------------------------a" << endl ;

            i = list.size() - 1 ;
            // cout << list.at( i ).scope << " \\++--SCOPE--++// b" << endl ;
            Node *b = new Node ;            
            if ( list.size() != 0 && scopeoflast - list.at( i ).scope < 2 && !hasquotefront ) {
              twonode = true ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( ( Onlyone( list ) && scopeoflast != list.at( i ).scope ) 
                        || list.at( i - 1 ).scope == list.at( i ).scope ) 
                   && list.at( i ).node -> content != "\'" ) {
                Node *br = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *bl = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( b, "", bl, br ) ;
              } // if
              else {
                b = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
              // Preorder( b ) ; 
            } // if
            // cout << "--------------------------b" << endl ;

            if ( twonode && onenode ) {
              twonode = false ;
              onenode = false ;
              Node *temphead = new Node ;
              NewNode2( temphead, "", b, a ) ;
              temp.node = temphead ;
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // if
            else if ( onenode ) {
              onenode = false ;
              temp.node = a ; 
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // else if
          } // else 
          // Preorder( temp.node ) ; 

          i = list.size() - 1 ;
          // cout << i + 1 << "   ***---size---***   " << endl ;
        } // while

        first = true ;
        list.at( list.size() - 1 ).scope-- ;
        list.at( list.size() - 1 ).combine = true ;
        hasquotefront = false ;
        numofl-- ;
      } // else if
      else if ( mtokenstring.at( i ) == "." ) {
        dotlist.push_back( numofl ) ;  
      } // else if
      else {
        Node *atom = new Node ;
        if ( mtokenstring.at( i ) == "\'" && !quote && !qquote ) {
          if ( mtokenstring.at( i + 1 ) == "(" )
            qquote = true ;
          else
            qquotesingle = true ;
        } // if

        if ( i > 0 && mtokenstring.at( i - 1 ) != "define" && !quote && !qquote &&
             !qquotesingle && !IsDefine( mtokenstring.at( i - 1 ) ) ) { 
          atom = ReturnNodeInNodeList( mtokenstring.at( i ) ) ;
          if ( atom == NULL && mtokenstring.at( i - 1 ) != "(" )
            atom = ReturnNodeInFunctionList( mtokenstring.at( i ) ) ;
        } // if 
        else
          atom = NULL ;

        if ( atom == NULL ) {
          atom = new Node ;
          NewNode( atom, mtokenstring.at( i ) ) ;
        } // if
        
        temp.node = atom ;
        // Preorder( temp.node, false, false, false, 0 ) ;
        temp.scope = numofl ;
        temp.combine = false ;
        list.push_back( temp ) ;

        if ( i > 0 && mtokenstring.at( i - 1 ) == "(" && 
             ( mtokenstring.at( i ) == "quote" || IsQuote( mtokenstring.at( i ) ) ) && !quote &&
             !qquote ) {
          quote = true ;
          quotescope = numofl ;
        } // if

        if ( i > 0 && qquotesingle && mtokenstring.at( i - 1 ) == "\'" )
          qquotesingle = false ;

      } // else
    } // for

    // cout << list.size() << "+++++++++++++++++++++0 " << endl ;

    while ( list.size() > 1 ) {
      int i = list.size() - 1 ;
      Node *br = list.at( i ).node ;
      list.pop_back() ;
      i-- ;      
      Node *bl = list.at( i ).node ;
      list.pop_back() ;
      i-- ;      
      Node *b = new Node ;
      NewNode2( b, "", bl, br ) ;
      temp.node = b ;
      temp.scope = numofl ;
      list.push_back( temp ) ;
    } // while 

    mnodelist.push_back( list.at( 0 ).node ) ;
    // cout << list.size() << " size " << endl ;
    // Preorder( list.at( 0 ).node, false, false, false, 0 ) ;  
  } // BuildTree()

  void BuildTree2() {
    vector< ForTree > list ;
    ForTree temp ;
    vector< int > dotlist ;
    bool first = true, twonode = false, onenode = false ;
    bool qfirst = true, qquotesingle = false ;
    bool hasquotefront = false, quote = false, qquote = false ;
    int numofl = 0, scopeoflast = 0, quoteRP = 0 ;
    int quoteargument = 0, quotescope = 0 ;
    for ( int i = 0 ; i < mtokenstring.size() ; i++ ) {
      if ( mtokenstring.at( i ) == "(" ) {
        numofl++ ;

        if ( qquote )
          quotescope = numofl ;
      } // if
      else if ( mtokenstring.at( i ) == ")" ) {
        if ( numofl == quotescope ) {
          quote = false ;
          qquote = false ;
        } // if

        if ( list.size() == 1 && mtokenstring.size() == 3 ) {
          Node *h = new Node ;
          NewNode2( h, "", list.at( 0 ).node, NULL ) ;
          temp.node = h ;
          temp.scope = numofl ;
          list.pop_back() ;
          list.push_back( temp ) ;
        } // if
        else if ( Onlyone( list ) ) { // Deal with () has only one atom. eg : (1)  
          // cout << list.at( list.size() - 1 ).node -> content << endl ;
          // cout << list.at( list.size() - 1 ).scope << endl ;
          Node *h = new Node ;
          NewNode2( h, "", list.at( list.size() - 1 ).node, NULL ) ;
          temp.node = h ;
          temp.scope = numofl ; 
          list.pop_back() ;
          list.push_back( temp ) ;
          first = false ;
        } // if

        int i = list.size() - 1 ; 
        while ( i > 0 && ( list.at( i - 1 ).scope == numofl || 
                           ( ( list.at( i - 1 ).node -> content == "\'" ) 
                             && list.at( i - 1 ).scope == list.at( i ).scope - 1 ) ) 
                && list.at( i - 1 ).scope != 0 ) {
          if ( list.at( i - 1 ).node -> content == "\'" && 
               list.at( i - 1 ).scope == list.at( i ).scope - 1  ) {
            hasquotefront = true ;
          } // if

          if ( dotlist.size() != 0 && dotlist.at( dotlist.size() - 1 ) == numofl ) {
            dotlist.pop_back() ;
            Node *a = new Node ;
            if ( list.size() != 0 ) {
              onenode = true ;
              scopeoflast = list.at( i ).scope ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( Onlyone( list ) || list.at( i - 1 ).scope == list.at( i ).scope ) ) {
                Node *ar = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *al = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( a, "", al, ar ) ;
              } // if
              else {
                a = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
            } // if
            // Preorder( a ) ; 
            // cout << "--------------------------a" << endl ;

            i = list.size() - 1 ;
            // cout << list.at( i ).scope << " \\++--SCOPE--++// b" << endl ;
            Node *b = new Node ;            
            if ( list.size() != 0 && scopeoflast - list.at( i ).scope < 2 && !hasquotefront ) {
              twonode = true ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( ( Onlyone( list ) && scopeoflast != list.at( i ).scope ) 
                        || list.at( i - 1 ).scope == list.at( i ).scope ) 
                   && list.at( i ).node -> content != "\'" ) {
                Node *br = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *bl = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( b, "", bl, br ) ;
              } // if
              else {
                b = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
              // Preorder( b ) ; 
            } // if
            // cout << "--------------------------b" << endl ;

            if ( twonode && onenode ) {
              twonode = false ;
              onenode = false ;
              Node *temphead = new Node ;
              NewNode2( temphead, "", b, a ) ;
              temp.node = temphead ;
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // if
            else if ( onenode ) {
              onenode = false ;
              temp.node = a ; 
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // else if

            first = false ;
          } // if
          else {
            if ( first ) {
              first = false ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0 ) {
                Node *b = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                while ( i >= 0 && list.at( i ).node -> content == "\'" && list.at( i ).scope == numofl ) {
                  Node *bl = list.at( i ).node ;
                  list.pop_back() ;
                  i-- ; 
                  Node *a = b ;
                  b = new Node ;
                  NewNode2( b, "", bl, a ) ;
                } // while

                Node *h = new Node ;
                NewNode2( h, "", b, NULL ) ;
                temp.node = h ;
                temp.scope = numofl ;
                list.push_back( temp ) ;
                // cout << "quote first" << endl ;
              } // if
              else {
                Node *h = new Node ;
                NewNode2( h, "", list.at( i ).node, NULL ) ;
                temp.node = h ;
                temp.scope = numofl ;
                list.pop_back() ;
                list.push_back( temp ) ;
                // cout << "aaaa" << list.size() << endl ;
              } // else
            } // if
            // Preorder( list.at( i ).node ) ;

            i = list.size() - 1 ;
            // cout << list.at( i ).scope << " \\++--SCOPE--++// a" << endl ;
            Node *a = new Node ;
            if ( list.size() != 0 ) {
              onenode = true ;
              scopeoflast = list.at( i ).scope ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( Onlyone( list ) || list.at( i - 1 ).scope == list.at( i ).scope ) ) {
                Node *ar = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *al = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( a, "", al, ar ) ;
              } // if
              else {
                a = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
            } // if
            // Preorder( a ) ; 
            // cout << "--------------------------a" << endl ;

            i = list.size() - 1 ;
            // cout << list.at( i ).scope << " \\++--SCOPE--++// b" << endl ;
            Node *b = new Node ;            
            if ( list.size() != 0 && scopeoflast - list.at( i ).scope < 2 && !hasquotefront ) {
              twonode = true ;
              if ( i > 0 && list.at( i - 1 ).node -> content == "\'" && list.at( i - 1 ).scope != 0
                   && ( ( Onlyone( list ) && scopeoflast != list.at( i ).scope ) 
                        || list.at( i - 1 ).scope == list.at( i ).scope ) 
                   && list.at( i ).node -> content != "\'" ) {
                Node *br = list.at( i ).node ;
                list.pop_back() ;
                i-- ;      
                Node *bl = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
                NewNode2( b, "", bl, br ) ;
              } // if
              else {
                b = list.at( i ).node ;
                list.pop_back() ;
                i-- ;
              } // else
              // Preorder( b ) ; 
            } // if
            // cout << "--------------------------b" << endl ;

            if ( twonode && onenode ) {
              twonode = false ;
              onenode = false ;
              Node *temphead = new Node ;
              NewNode2( temphead, "", b, a ) ;
              temp.node = temphead ;
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // if
            else if ( onenode ) {
              onenode = false ;
              temp.node = a ; 
              temp.scope = numofl ;
              list.push_back( temp ) ; 
            } // else if
          } // else 
          // Preorder( temp.node ) ; 

          i = list.size() - 1 ;
          // cout << i + 1 << "   ***---size---***   " << endl ;
        } // while

        first = true ;
        list.at( list.size() - 1 ).scope-- ;
        list.at( list.size() - 1 ).combine = true ;
        hasquotefront = false ;
        numofl-- ;
      } // else if
      else if ( mtokenstring.at( i ) == "." ) {
        dotlist.push_back( numofl ) ;  
      } // else if
      else {
        Node *atom = new Node ;
        NewNode( atom, mtokenstring.at( i ) ) ;
        temp.node = atom ;
        // Preorder( temp.node, false, false, false, 0 ) ;
        temp.scope = numofl ;
        temp.combine = false ;
        list.push_back( temp ) ;
      } // else
    } // for

    // cout << list.size() << "+++++++++++++++++++++0 " << endl ;

    while ( list.size() > 1 ) {
      int i = list.size() - 1 ;
      Node *br = list.at( i ).node ;
      list.pop_back() ;
      i-- ;      
      Node *bl = list.at( i ).node ;
      list.pop_back() ;
      i-- ;      
      Node *b = new Node ;
      NewNode2( b, "", bl, br ) ;
      temp.node = b ;
      temp.scope = numofl ;
      list.push_back( temp ) ;
    } // while 

    mnotElist.push_back( list.at( 0 ).node ) ;
    // cout << list.size() << " size " << endl ;
    // Preorder( list.at( 0 ).node, false, false, false, 0 ) ;  
  } // BuildTree2()
  
  void BuildANode() {
    Node *temp = new Node ;
    Node *left, *right ;
    for ( int i = 0 ; i < mtokenstring.size() ; i++ ) {
      if ( mtokenstring[ i ] != "\'" ) {
        NewNode( temp, mtokenstring[ i ] ) ;
        mtokenstring.pop_back() ;
      } // if
    } // for

    while ( mtokenstring.size() > 0 ) {
      right = new Node ;
      right = temp ;
      left = new Node ;
      NewNode( left, "\'" ) ;
      temp = new Node ;
      NewNode2( temp, "", left, right ) ;
      mtokenstring.pop_back() ; 
    } // while
    // Preorder( temp, false, false, false, 0 ) ;
    mnodelist.push_back( temp ) ;
    mnotElist.push_back( temp ) ;
  } // BuildANode()

  void PrettyPrint( vector<string> mtokenstring, int loc, int numofl, int spaceloc, 
                    int cancelL, int x, int dotl ) {
    if ( loc == mtokenstring.size() - 1 ) {
      if ( mtokenstring.at( loc ) == ")" ) {
        for ( int i = spaceloc - 2 ; i > 0 ; i-- ) {
          cout << " " ;
          x++ ;
        } // for

        cout << ")" << endl ;
        x = 0 ;
        spaceloc = spaceloc - 2 ;
        numofl-- ;

        if ( mquoteloc.size() != 0 && mquoteloc.at( mquoteloc.size() - 1 ) != 500 ) {
          for ( int i = mquoteloc.size() ; i > 0 ; i-- ) { 
            spaceloc = spaceloc - 2 ;
            for ( int i = spaceloc ; i > 0 ; i-- ) 
              cout << " " ;

            cout << ")" << endl ;
          } // for

          mquoteloc.clear() ;
        } // if  
      } // if 
    } // if 
    else if ( mtokenstring.at( loc ) == "(" && loc == 0 ) {
      numofl++ ;
      for ( int i = spaceloc - x ; i > 0 ; i-- ) {
        cout << " " ;
        x++ ;
      } // for

      cout << "( " ;
      x = x + 2 ;
      spaceloc = spaceloc + 2 ;
      loc++ ;
      PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
    } // else if
    else if ( mtokenstring.at( loc ) == "\'" ) { 
      for ( int i = spaceloc - x ; i > 0 ; i-- ) 
        cout << " " ;

      cout << "( quote" << endl ;
      x = 0 ;
      spaceloc = spaceloc + 2 ;
      loc++ ;
      if ( mquotefirst ) {
        mquoteloc.clear() ;
        mquotefirst = false ;
      } // if

      mquoteloc.push_back( numofl ) ;
      PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
    } // else if
    else if ( mtokenstring.at( loc ) == "(" && ( mquoteloc.at( mquoteloc.size() - 1 ) != numofl
                                                 || mtokenstring.at( loc - 1 ) != ")" ) ) {
      numofl++ ;
      for ( int i = spaceloc - x ; i > 0 ; i-- ) {
        cout << " " ;
        x++ ;
      } // for

      cout << "( " ;
      x = x + 2 ;
      spaceloc = spaceloc + 2 ;
      loc++ ;
      if ( mfirst ) {
        mfirst = false ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
      } // if
      else if ( mtokenstring.at( loc - 1 ) == "(" ) {
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
      } // else if
      else {
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
      } // else
    } // else if
    else if ( mquoteloc.size() > 0 && mquoteloc.at( mquoteloc.size() - 1 ) == numofl ) {
      if ( mtokenstring.at( loc - 1 ) == "\'" ) {
        for ( int i = spaceloc ; i > 0 ; i-- ) {
          cout << " " ;
          x++ ;
        } // for

        cout << mtokenstring.at( loc ) << endl ;
        loc++ ;
      } // if

      spaceloc = spaceloc - 2 ;

      for ( int i = spaceloc ; i > 0 ; i-- ) 
        cout << " " ;

      cout << ")" << endl ;
      x = 0 ;
      mquoteloc.pop_back() ;

      if ( mquoteloc.size() == 0 )
        mquoteloc.push_back( 500 ) ;
      PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
    } // else if
    else if ( mtokenstring.at( loc ) == ")" ) {
      if ( cancelL > 0 && dotl == numofl ) {
        loc++ ;
        cancelL-- ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
      } // if
      else if ( mtokenstring.at( loc + 1 ) != ")" && mtokenstring.at( loc + 1 ) != "." ) { // maybe quote
        for ( int i = spaceloc - 2 ; i > 0 ; i-- ) {
          cout << " " ;
          x++ ;
        } // for

        cout << ")" << endl ;
        x = 0 ;
        spaceloc = spaceloc - 2 ;
        loc++ ;
        numofl-- ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
      } // else if
      else {
        for ( int i = spaceloc - 2 ; i > 0 ; i-- ) {
          cout << " " ;
          x++ ;
        } // for

        cout << ")" << endl ;
        x = 0 ;
        spaceloc = spaceloc - 2 ;
        numofl-- ;
        loc++ ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
      } // else
    } // else if
    else if ( mtokenstring.at( loc ) == "." ) {
      if ( mtokenstring.at( loc + 1 ) == "(" ) {
        cancelL++ ;
        dotl = numofl ;
        loc = loc + 2 ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;  
      } // if
      else if ( mtokenstring.at( loc - 1 ) == ")" ) {
        if ( mtokenstring.at( loc + 1 ) != "nil" && mtokenstring.at( loc + 1 ) != "#f" ) {
          for ( int i = spaceloc ; i > 0 ; i-- ) {
            cout << " " ;
            x++ ;
          } // for

          cout << "." << endl ;
          x = 0 ;
        } // if
        // walk = mlist.at( numofl - 1 ) ;

        if ( mtokenstring.at( loc + 1 ) == "(" )
          loc = loc + 2 ;
        else
          loc++ ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ;
      } // else if
      else {
        if ( mtokenstring.at( loc + 1 ) != "nil" && mtokenstring.at( loc + 1 ) != "#f" ) {
          for ( int i = spaceloc ; i > 0 ; i-- ) {
            cout << " " ;
            x++ ;
          } // for

          cout << "." << endl ;
          x = 0 ;
        } // if

        loc++ ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ; 
      } // else
    } // else if
    else {
      if ( mtokenstring.at( loc - 1 ) == "." && mtokenstring.at( loc ) != "#f"
           && mtokenstring.at( loc ) != "nil" ) {
        for ( int i = spaceloc ; i > 0 ; i-- ) {
          cout << " " ;
          x++ ;
        } // for

        cout << mtokenstring.at( loc ) << endl ;
        x = 0 ;
        loc++ ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ; 
      } // if
      else if ( mtokenstring.at( loc - 1 ) != "." ) {
        for ( int i = spaceloc - x ; i > 0 ; i-- ) {
          cout << " " ;
          x++ ;
        } // for

        cout << mtokenstring.at( loc ) << endl ;
        x = 0 ;
        loc++ ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ; 
      } // else if
      else {
        loc++ ;
        PrettyPrint( mtokenstring, loc, numofl, spaceloc, cancelL, x, dotl ) ; 
      } // else
    } // else
  } // PrettyPrint()

  void DoubleQuote() {
    bool check = false ;
    scanf( "%c", &mread ) ;
    mx++ ;
    while ( ( mread != '\"' && mread != '\n' ) ) {
      if ( mread != '\\' )
        mtoken = mtoken + mread ;
      else if ( mread == '\\' ) {
        if ( cin.peek() == 'n' ) {
          scanf( "%c", &mread ) ;
          mx++ ;
          mtoken = mtoken + "\n" ;
        } // if
        else if ( cin.peek() == 't' ) {
          scanf( "%c", &mread ) ;
          mx++ ;
          mtoken = mtoken + "\t" ;
        } // else if
        else if ( cin.peek() == '\\' ) {
          scanf( "%c", &mread ) ;
          mx++ ;
          mtoken = mtoken + "\\" ;
        } // else if
        else if ( cin.peek() == '\"' ) {
          scanf( "%c", &mread ) ;
          mx++ ;
          mtoken = mtoken + "\"" ;
        } // else if
        else
          mtoken = mtoken + mread ;
      } // else if

      if ( cin.peek() != EOF ) {
        scanf( "%c", &mread ) ;
        mx++ ;
      } // if

      // if ( cin.peek() == EOF )
        // mtokenstring[100] = "" ;
    } // while

    if ( mread == '\"' ) {
      mtoken = mtoken + mread ;
      check = true ;
    } // if

    if ( !check ) {
      mnoclosingquote = true ;
      mtoken = "" ;
    } // if

    check = false ;
  } // DoubleQuote()

  void Identify() {
    char temp[100] ;
    bool negative = false, flo = false ;
    int iNt = 0, div = 1 ;
    float fLoat = 0 ;
    for ( int i = 0 ; i < mtoken.size() ; i++ ) {
      if ( mtoken.at( i ) == '+' || mtoken.at( i ) == '-' ) {
        if ( mtoken.at( i ) == '-' ) 
          negative = true ;
      } // if
      else if ( mtoken.at( i ) >= '0' && mtoken.at( i ) <= '9' ) {
        if ( flo == false  )
          fLoat = fLoat * 10 + mtoken.at( i ) - '0' ;
        else {
          fLoat = fLoat + ( mtoken.at( i ) - '0' ) / pow( 10, div ) ;
          div++ ;
        } // else
      } // else if
      else if ( mtoken.at( i ) == '.' ) {
        flo = true ;
      } // else if 
      else
        cout << "Error NUM" << endl ;
    } // for

    if ( negative == true )
      fLoat = -1 * fLoat ;
    if ( flo == false ) {
      iNt = fLoat ;
      sprintf( temp, "%d ", iNt ) ;
      mtoken = "" ;
      for ( int i = 0 ; i < strlen( temp ) - 1 ; i++ )
        mtoken = mtoken + temp[i] ;
    } // if
    else {
      sprintf( temp, "%.3f ", fLoat ) ;
      mtoken = "" ;
      for ( int i = 0 ; i < strlen( temp ) - 1 ; i++ )
        mtoken = mtoken + temp[i] ;
    } // else
  } // Identify()

  string Identify2( string str ) {
    bool nonnum = true ;
    char temp[100] ;
    bool negative = false, flo = false ;
    int iNt = 0, div = 1 ;
    int pl = 0, mi = 0, dot = 0 ;
    float fLoat = 0 ;
    for ( int i = 0 ; i < str.size() ; i++ ) {
      if ( str.at( i ) == '+' )
        pl++ ;
      else if ( str.at( i ) == '-' )
        mi++ ;
      else if ( str.at( i ) == '.' )
        dot++ ;
      else if ( str.at( i ) >= '0' && str.at( i ) <= '9' )
        nonnum = false ;
    } // for

    if ( str == "." || pl > 1 || mi > 1 || dot > 1 )
      nonnum = true ;
      
    for ( int i = 0 ; i < str.size() ; i++ ) {
      if ( str.at( i ) == '+' || str.at( i ) == '-' ) {
        if ( i != 0 )
          return str ; 
        else if ( str.at( i ) == '-' ) 
          negative = true ;
      } // if
      else if ( str.at( i ) >= '0' && str.at( i ) <= '9' ) {
        if ( flo == false  )
          fLoat = fLoat * 10 + str.at( i ) - '0' ;
        else {
          fLoat = fLoat + ( str.at( i ) - '0' ) / pow( 10, div ) ;
          div++ ;
        } // else
      } // else if
      else if ( str.at( i ) == '.' ) {
        flo = true ;
      } // else if 
      else
        return str ;
    } // for

    if ( negative == true && !nonnum )
      fLoat = -1 * fLoat ;

    if ( flo == false && !nonnum ) {
      iNt = fLoat ;
      sprintf( temp, "%d ", iNt ) ;
      str = "" ;
      for ( int i = 0 ; i < strlen( temp ) - 1 ; i++ )
        str = str + temp[i] ;
    } // if
    else if ( !nonnum ) {
      sprintf( temp, "%.3f ", fLoat ) ;
      str = "" ;
      for ( int i = 0 ; i < strlen( temp ) - 1 ; i++ )
        str = str + temp[i] ;
    } // else if
    
    return str ;
  } // Identify2()

  string ConvertIntoStr( float result ) {
    char temp[100] ;
    string out = "" ;
    bool negative = false, flo = false ;
    int iNt = 0 ;
    float fLoat = 0 ;

    if ( result < 0 )
      negative = true ;
    if ( mfloat )
      flo = true ;

    if ( negative == true )
      fLoat = -1 * fLoat ;
    if ( flo == false ) {
      iNt = result ;
      sprintf( temp, "%d ", iNt ) ;
      for ( int i = 0 ; i < strlen( temp ) - 1 ; i++ )
        out = out + temp[i] ;
    } // if
    else {
      sprintf( temp, "%.3f ", result ) ;
      for ( int i = 0 ; i < strlen( temp ) - 1 ; i++ )
        out = out + temp[i] ;
    } // else

    return out ;
  } // ConvertIntoStr()

  void Number() {
    mtoken = mtoken + mread ;
    bool dot = false, onlydot = false, onlysign = false ;
    bool nonnum = true, quote = false, read = false ;
    if ( mread == '.' && ( cin.peek() == '\n' || cin.peek() == ' ' ||
                           cin.peek() == '\t' ) ) 
      onlydot = true ;
    else if ( ( mread == '+' || mread == '-' ) && ( cin.peek() == '\n' || cin.peek() == ' ' ||
                                                    cin.peek() == '\t' ) )
      onlysign = true ;
    else if ( mread == '.' )
      dot =  true ;
    if ( mtoken >= "0" && mtoken <= "9" )
      nonnum = false ;

    if ( cin.peek() != '\"' && cin.peek() != '\'' && cin.peek() != '('
         && cin.peek() != ')' && cin.peek() != EOF && cin.peek() != ';' ) {
      scanf( "%c", &mread ) ;
      if ( mread != '\n' && mread != ' ' && mread != '\t' )
        mx++ ;
      read = true ;
    } // if
    else {
      quote = true ;
      mstringM = true ;
    } // else

    while ( ( ( mread >= '0' && mread <= '9' ) || ( mread == '.' && dot == false ) ) && !quote ) { 
      if ( mread >= '0' && mread <= '9' ) {
        mtoken = mtoken + mread ;
        nonnum = false ;
      } // if
      else if ( mread == '.' ) {
        dot = true ;
        mtoken = mtoken + mread ;
      } // else if

      if ( cin.peek() != '\"' && cin.peek() != '\'' && cin.peek() != '('
           && cin.peek() != ')' && cin.peek() != EOF && cin.peek() != ';' ) {       
        scanf( "%c", &mread ) ;
        mx++ ;
      } // if
      else {
        quote = true ;
        mstringM = true ;
      } // else
    } // while

    if ( ( mread == '\n' || mread == ' ' || mread == '\t' || quote ) && !onlydot && !onlysign && !nonnum )
      Identify() ;
    else if ( onlydot ) {
      mbehinddot = true ;
      mnumofAtom = 0 ;
      mdot++ ;
      if ( mdot > 1 )
        mdoterror = true ;
    } // else if
    else {
      if ( mread != ' ' && mread != '\n' && mread != '\t' ) {
        if ( mtoken == "." && !read )
          mdoterror = true ; 
        else if ( !onlysign )
          mtoken = mtoken + mread ;

        mnum = false ;
      } // if
    } // else

    dot = false ;
  } // Number()

  bool Syntax() {
    int dotnum = 0, atomafterdot = 0, atomnum = 0 ;
    int afterquote = 0, quotenum = 0 ;
    bool check ;
    for ( ; merrorcheck_loc < merrorcheck.size() ; merrorcheck_loc++ ) {
      if ( merrorcheck.at( merrorcheck_loc ).type == "LP" ) {
        if ( atomafterdot == 1 ) {
          cout << "> ERROR (unexpected token) : ')' expected when token at Line "
               << merrorcheck.at( merrorcheck_loc ).locy << " Column "
               << merrorcheck.at( merrorcheck_loc ).locx << " is >>"
               << mtokenstring.at( merrorcheck_loc ) << "<<" << endl << endl ;

          return false ;
        } // if
        else {
          merrorcheck_loc++ ;
          check = Syntax() ; 
          if ( !check ) {
            return false ;
          } // if
          else if ( check && merrorcheck_loc == merrorcheck.size() - 1 ) {
            return true ;
          } // else if
          else {
            atomnum++ ;
            if ( dotnum == 1 )
              atomafterdot++ ;
          } // else
        } // else
      } // if
      else if ( merrorcheck.at( merrorcheck_loc ).type == "DOT" ) {
        dotnum++ ;
        if ( atomnum == 0 || ( merrorcheck_loc > 0 && 
                               merrorcheck.at( merrorcheck_loc - 1 ).type == "QUOTE" ) ||
             ( merrorcheck_loc > 0 && merrorcheck.at( merrorcheck_loc - 1 ).type == "DOT" ) ) {
          cout << "> " << "ERROR (unexpected token) : atom or '(' expected when token at Line " 
               << merrorcheck.at( merrorcheck_loc ).locy << " Column " 
               << merrorcheck.at( merrorcheck_loc ).locx
               << " is >>.<<" << endl << endl ;

          return false ; 
        } // if
        else if ( dotnum > 1 ) {
          cout << "> " << "ERROR (unexpected token) : ')' expected when token at Line " 
               << merrorcheck.at( merrorcheck_loc ).locy
               << " Column " << merrorcheck.at( merrorcheck_loc ).locx << " is >>.<<" << endl << endl ;

          return false ;
        } // else if
      } // else if
      else if ( merrorcheck.at( merrorcheck_loc ).type == "ATOM" ) {
        atomnum++ ;
        if ( dotnum == 1 ) {
          atomafterdot++ ;
          if ( atomafterdot > 1 ) {
            cout << "> " << "ERROR (unexpected token) : ')' expected when token at Line " 
                 << merrorcheck.at( merrorcheck_loc ).locy
                 << " Column " << merrorcheck.at( merrorcheck_loc ).locx << " is >>" 
                 << mtokenstring.at( merrorcheck_loc )
                 << "<<" << endl << endl ;

            return false ;
          } // if
        } // if
      } // else if
      else if ( merrorcheck.at( merrorcheck_loc ).type == "RP" 
                && merrorcheck_loc != merrorcheck.size() - 1  ) {
        if ( dotnum == 1 && atomafterdot == 0 ) {
          cout << "> " << "ERROR (unexpected token) : atom or '(' expected when token at Line "
               << merrorcheck.at( merrorcheck_loc ).locy << " Column " 
               << merrorcheck.at( merrorcheck_loc ).locx 
               << " is >>)<<" << endl << endl ;
               
          return false ;
        } // if

        return true ;

      } // else if 
      else if ( merrorcheck_loc == merrorcheck.size() - 1 
                && merrorcheck.at( merrorcheck_loc ).type == "RP" ) {
        if ( dotnum == 1 && atomafterdot == 0 ) {
          cout << "> " << "ERROR (unexpected token) : atom or '(' expected when token at Line " 
               << merrorcheck.at( merrorcheck_loc ).locy << " Column " 
               << merrorcheck.at( merrorcheck_loc ).locx 
               << " is >>)<<" << endl << endl ;
               
          return false ;
        } // if

        return true ;
      } // else if
    } // for

    return true ;
  } // Syntax() 

  void BuildFunctionList() {
    ForDefineSymbol temp ;

    temp.name = "define" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure define>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "clean-environment" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure clean-environment>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "cons" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure cons>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "list" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure list>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "quote" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure quote>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "car" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure car>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "cdr" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure cdr>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "atom?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure atom?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "pair?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure pair?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "list?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure list?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "null?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure null?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "integer?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure integer?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "real?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure real?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "number?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure number?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "string?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure string?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "boolean?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure boolean?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "symbol?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure symbol?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "+" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure +>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "-" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure ->" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "*" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure *>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "/" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure />" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "not" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure not>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "and" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure and>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "or" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure or>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = ">" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure >>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "<" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure <>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = ">=" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure >=>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "<=" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure <=>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "=" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure =>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "string-append" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure string-append>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "string>?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure string>?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "string<?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure string<?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "string=?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure string=?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "eqv?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure eqv?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "equal?" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure equal?>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "if" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure if>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "cond" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure cond>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "begin" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure begin>" ) ;
    mfunctionlist.push_back( temp ) ;

    temp.name = "exit" ;
    temp.node = new Node ;
    NewNode( temp.node, "#<procedure exit>" ) ;
    mfunctionlist.push_back( temp ) ;
  } // BuildFunctionList()
 
  bool IsInt( string str ) {
    if ( str == "" ) 
      return false ;
    for ( int i = 0 ; i < str.size() ; i++ ) {
      if ( i == 0 && ( str.at( i ) == '+' || str.at( i ) == '-' ) )
        ;
      else if ( str.at( i ) < '0' || str.at( i ) > '9' ) {
        return false ;
      } // if
    } // for

    return true ;
  } // IsInt()

  bool IsReal( string str ) {
    int dotnum = 0 ;
    if ( str == "" || str.size() < 5 ) 
      return false ;
    for ( int i = 0 ; i < str.size() ; i++ ) {
      if ( str.at( i ) == '.' ) {
        dotnum++ ;
        if ( dotnum > 1 )
          return false ;
      } // if
      else if ( i == 0 && str.at( i ) == '-' )
        ;
      else if ( str.at( i ) < '0' || str.at( i ) > '9' ) {
        return false ;
      } // if
    } // for

    return true ;
  } // IsReal()

  string ReturnContent( Node *temp ) {
    return temp -> left -> content ;
  } // ReturnContent()

  Node *Define( Node *temp, Node *non, int stage ) {
    Node *re = new Node ;
    Node *two = new Node ;
    Node *a = new Node ;
    bool isfunction = false ;
    int nonsense = 0 ;
    ForDefineSymbol gtemp, noteval ;
    if ( stage == 1 ) {
      mtempsymbol.name = temp -> left -> content ;
      mnoteval.name = non -> left -> content ;
      if ( IsInt( mtempsymbol.name ) || IsReal( mtempsymbol.name ) ||
           mtempsymbol.name == "nil" || mtempsymbol.name == "" ||
           mtempsymbol.name == "true" ||  mtempsymbol.name.at( 0 ) == '\"' ||
           IsFun( mtempsymbol.name ) ) {
        cout << "ERROR (DEFINE format) : " ;
        re = new Node ;
        NewNode( re, "FormatError" ) ;
        return re ;
      } // if
      else if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Define( temp -> right, non -> right, 2 ) ;
    } // if
    else if ( stage == 2 ) {
      mnoteval.node = new Node ;
      NewNode2( mnoteval.node, "{DEFINE}", non -> left, NULL ) ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" ) 
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;

        NewNode2( two, "{DEFINE}", a, NULL ) ;
        mtempsymbol.node = two ;
      } // if 
      else {
        a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( temp -> left != NULL && !InNodeListNotFix( ReturnContent( temp ), nonsense ) &&
                  !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << ReturnContent( temp ) << endl ;
          return NULL ;
        } // else if

        NewNode2( two, "{DEFINE}", a, NULL ) ;
        mtempsymbol.node = two ;
      } // else 

      if ( InFunctionList( two -> left -> content ) )
        isfunction = true ;

      if ( temp -> right != NULL && ( temp -> right != NULL && temp -> right -> content != "nil" ) ) {
        return NULL ;
      } // if  
      else {
        Node *a = new Node ;
        NewNode( a, mtempsymbol.name + " defined" ) ;
        if ( isfunction ) {
          if ( FixDefineFunctionList( mtempsymbol.name, mtempsymbol.node ) )
            ;
          else
            mdefinefunctionlist.push_back( mtempsymbol ) ;
        } // if

        if ( InNodeList( mtempsymbol.name, mtempsymbol.node ) )
          ;
        else
          mdefinelist.push_back( mtempsymbol ) ;

        for ( int i = 0 ; i < mdefinefunctionlist.size() ; i++ ) {
          if ( mtempsymbol.name == mdefinefunctionlist.at( i ).name && 
               mtempsymbol.node != mdefinefunctionlist.at( i ).node ) {
            gtemp = mdefinefunctionlist.at( i ) ;
            mdefinefunctionlist.at( i ) = mdefinefunctionlist.at( mdefinefunctionlist.size() - 1 ) ;
            mdefinefunctionlist.at( mdefinefunctionlist.size() - 1 ) = gtemp ;
            mdefinefunctionlist.pop_back() ;
          } // if
        } // for

        if ( InNotEval( mnoteval.name, mnoteval.node ) )
          ;
        else
          mnotevallist.push_back( mnoteval ) ;
        // cout << mdefinelist.size() << " F   DF " << mdefinefunctionlist.size() << endl ;
        return a ;
      } // else
    } // else if

    return re ;
  } // Define()

  Node *Cons( Node *temp, int stage, Node *dummy_root ) {
    Node *re = new Node ;
    int non = 0 ;
    if ( stage == 1 ) {
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          dummy_root -> left = EvalExp( temp -> left, NULL, false ) ;
        else
          dummy_root -> left = EvalExp( temp -> left, NULL, true ) ;

        if ( dummy_root -> left == NULL ) 
          return NULL ;
      } // if
      else { 
        dummy_root -> left = temp -> left ;

        if ( !InNodeListNotFix( dummy_root -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << dummy_root -> left -> content << endl ; 

          return NULL ;
        } // if
      } // else
 
      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Cons( temp -> right, 2, dummy_root ) ;
    } // if
    else if ( stage == 2 ) {
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          dummy_root -> right = EvalExp( temp -> left, NULL, false ) ;
        else
          dummy_root -> right = EvalExp( temp -> left, NULL, true ) ;

        if ( dummy_root -> right == NULL ) 
          return NULL ;
      } // if
      else {
        dummy_root -> right = temp -> left ;

        if ( !InNodeListNotFix( dummy_root -> right -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << dummy_root -> right -> content << endl ; 

          return NULL ;
        } // if
      } // else

      return dummy_root ;
    } // else if

    return re ;
  } // Cons()

  Node *List( Node *temp, int stage ) {
    Node *re = new Node ; 
    int non = 0 ;

    if ( temp -> content != "" ) {
      cout << "ERROR " << stage << endl ;
      return NULL ; 
    } // if

    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a != NULL && re != NULL ) {
        temp -> left = a ;
      } // if
      else 
        return NULL ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
    } // else

    if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
      stage++ ;
      re = List( temp -> right, stage ) ;
      stage-- ;
    } // if

    if ( stage == 1 && re != NULL ) {
      return temp ;
    } // if

    return re ;
  } // List()

  Node *Car( Node *temp ) {
    Node *a = new Node ;
    int non = 0 ; 
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content != "" ) {
        cout << "ERROR (car with incorrect argument type) : " <<
                 a -> content << endl ;

        return NULL ; 
      } // else if
      else 
        return a -> left ;
    } // if

    if ( !InNodeListNotFix( temp -> left -> content, non ) &&
         !InFunctionList( ReturnContent( temp ) ) ) {
      cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

      return NULL ;
    } // if
    else
      cout << "ERROR (car with incorrect argument type) : " <<
               temp -> left -> content << endl ; 
 
    return NULL ; 
  } // Car()

  Node *Cdr( Node *temp ) {
    Node *a = new Node ;
    int non = 0 ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content != "" ) {
        cout << "ERROR (cdr with incorrect argument type) : " <<
                 a -> content << endl ;

        return NULL ; 
      } // else if
      else if ( a -> right == NULL ) {
        a -> right = new Node ;
        NewNode( a -> right, "nil" ) ;
      } // else if

      return a -> right ;
    } // if

    if ( !InNodeListNotFix( temp -> left -> content, non ) &&
         !InFunctionList( ReturnContent( temp ) ) ) {
      cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

      return NULL ;
    } // if
    else
      cout << "ERROR (cdr with incorrect argument type) : " <<
               temp -> left -> content << endl ; 
 
    return NULL ; 
  } // Cdr()

  Node *Pair( Node *temp ) {
    Node *re = new Node ;
    Node *a = new Node ;
    int non = 0 ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      if ( temp -> left -> content == "" )
        a = EvalExp( temp -> left, NULL, true ) ;
      else
        a = EvalExp( temp -> left, NULL, false ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content == "" )
        NewNode( re, "#t" ) ;
      else
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else {
        NewNode( re, "nil" ) ;
      } // else
    } // else

    return re ;
  } // Pair()

  Node *IsList( Node *temp, int stage ) {
    Node *re = new Node ;
    Node *a = new Node ; 
    int non = 0 ;
    if ( stage == 1 ) {
      stage++ ;
      if ( temp -> left -> content != "" && temp -> left -> content != "{DEFINE}" ) {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if

        re = new Node ;
        if ( temp -> left -> content == "nil" )
          NewNode( re, "#t" ) ;
        else
          NewNode( re, "nil" ) ;
      } // if
      else {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ; 

        re = IsList( a, stage ) ; 
      } // else  
    } // if
    else if ( stage == 2 && temp -> content != "" ) {
      stage++ ;
      re = new Node ;
      NewNode( re, "nil" ) ;
    } // else if
    else if ( temp -> right != NULL && temp -> right -> content != "" 
              && temp -> right -> content != "nil" ) {
      stage++ ;
      re = new Node ;
      NewNode( re, "nil" ) ;
    } // else if
    else {
      stage++ ;
      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = IsList( temp -> right, stage ) ;
    } // else

    stage-- ;
    if ( stage == 1 ) {
      if ( re -> content != "nil" ) {
        re = new Node ;
        NewNode( re, "#t" ) ;
      } // if
    } // if

    return re ;
  } // IsList()
  
  Node *Atom( Node *temp ) {
    Node *re = new Node ;
    int non = 0 ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content != "" )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( temp -> left -> content != "" ) {
        NewNode( re, "#t" ) ;
      } // else if
      else {
        NewNode( re, "nil" ) ;
      } // else
    } // else

    return re ;
  } // Atom()

  Node *Null( Node *temp ) {
    int non = 0 ;
    Node *re = new Node ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content == "nil" )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( temp -> left -> content == "nil" )
        NewNode( re, "#t" ) ;
      else
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // Null()

  Node *Int( Node *temp ) {
    int non = 0 ;
    Node *re = new Node ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( IsInt( a -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( IsInt( temp -> left -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // Int()

  Node *Real( Node *temp ) {
    int non = 0 ;
    Node *re = new Node ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( IsReal( a -> content ) || IsInt( a -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( IsReal( temp -> left -> content ) || IsInt( temp -> left -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // Real()

  Node *IsNumber( Node *temp ) {
    int non = 0 ;
    Node *re = new Node ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( IsReal( a -> content ) || IsInt( a -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( IsReal( temp -> left -> content ) || IsInt( temp -> left -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // IsNumber()

  Node *String( Node *temp ) {
    Node *re = new Node ;
    int non = 0 ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content != "" && a -> content.at( 0 ) == '\"' )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( temp -> left -> content != "" && temp -> left -> content.at( 0 ) == '\"' )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // String()

  Node *Boolean( Node *temp ) {
    int non = 0 ;
    Node *re = new Node ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content == "#t" || a -> content == "nil" )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( temp -> left -> content == "#t" || temp -> left -> content == "nil" )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // Boolean()

  Node *Symbol( Node *temp ) {
    int non = 0 ;
    Node *re = new Node ;
    int nonsense = 0 ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( !InDefine( temp -> left, nonsense ) && !IsInt( a -> content ) && !IsReal( a -> content ) 
                && a -> content.at( 0 ) != '\"' && a -> content != "nil" 
                && a -> content != "#t" && !IsFun( a -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) 
           && temp -> left -> content.at( 0 ) != '\"' && temp -> left -> content != "nil" 
           && temp -> left -> content != "#t" && !InFunctionList( temp -> left -> content ) )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // Symbol()

  Node *Arithmetic( Node *temp, int stage, int mode, float &result ) { // / a & eval 
    Node *re = new Node ;
    Node *a = new Node ; 
    int non = 0 ;
    if ( stage == 1 ) {
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          if ( mode == 0 ) {
            cout << "ERROR (+ with incorrect argument type) : " ; 
            Preorder( a, false, false, false, 0 ) ; 
          } // if
          else if ( mode == 1 ) {
            cout << "ERROR (- with incorrect argument type) : " ; 
            Preorder( a, false, false, false, 0 ) ; 
          } // else if
          else if ( mode == 2 ) {
            cout << "ERROR (* with incorrect argument type) : " ; 
            Preorder( a, false, false, false, 0 ) ; 
          } // else if
          else if ( mode == 4 ) {
            cout << "ERROR (/ with incorrect argument type) : " ; 
            Preorder( a, false, false, false, 0 ) ; 
          } // else if

          return NULL ;
        } // else if
        else
          result = ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          if ( mode == 0 )
            cout << "ERROR (+ with incorrect argument type) : " << temp -> left -> content << endl ;
          else if ( mode == 1 )
            cout << "ERROR (- with incorrect argument type) : " << temp -> left -> content << endl ;
          else if ( mode == 2 )
            cout << "ERROR (* with incorrect argument type) : " << temp -> left -> content << endl ;
          else if ( mode == 4 )
            cout << "ERROR (/ with incorrect argument type) : " << temp -> left -> content << endl ;
          return NULL ;
        } // else if
        else
          result = ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Arithmetic( temp -> right, stage, mode, result ) ;
    } // if
    else if ( mode == 0 ) { // +
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (+ with incorrect argument type) : " ; 
          Preorder( a, false, false, false, 0 ) ;
          return NULL ;
        } // else if
        else
          result = result + ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (+ with incorrect argument type) : " << temp -> left -> content << endl ;
          return NULL ;
        } // else if
        else
          result = result + ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Arithmetic( temp -> right, stage, 0, result ) ;
    } // else if
    else if ( mode == 1 ) { // -
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (- with incorrect argument type) : " ; 
          Preorder( a, false, false, false, 0 ) ;
          return NULL ;
        } // else if
        else
          result = result - ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (- with incorrect argument type) : " << temp -> left -> content << endl ;
          return NULL ;
        } // else if
        else
          result = result - ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Arithmetic( temp -> right, stage, 1, result ) ;
    } // else if
    else if ( mode == 2 ) { // *
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (* with incorrect argument type) : " ; 
          Preorder( a, false, false, false, 0 ) ;

          return NULL ;
        } // else if
        else
          result = result * ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (* with incorrect argument type) : " << temp -> left -> content << endl ;
          return NULL ;
        } // else if
        else
          result = result * ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Arithmetic( temp -> right, stage, 2, result ) ;
    } // else if
    else if ( mode == 3 ) { // / ---------------------------- Don't forget DIVISION BY 0 
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (/ with incorrect argument type) : " ; 
          Preorder( a, false, false, false, 0 ) ;

          return NULL ;
        } // else if
        else if ( a -> content == "0" ) {
          cout << "ERROR (division by zero) : /" << endl ;

          return NULL ;
        } // else if
        else
          result = result / ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (/ with incorrect argument type) : " << temp -> left -> content << endl ;

          return NULL ;
        } // else if
        else if ( temp -> left -> content == "0" ) {
          cout << "ERROR (division by zero) : /" << endl ;

          return NULL ;
        } // else if
        else
          result = result / ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Arithmetic( temp -> right, stage, 3, result ) ; 
    } // else if

    stage-- ;
    if ( stage == 1 && re != NULL ) {
      re = new Node ;
      NewNode( re, ConvertIntoStr( result ) ) ;
    } // if

    return re ;
  } // Arithmetic()

  Node *Not( Node *temp ) {
    Node *re = new Node ;
    int non = 0 ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content == "nil" )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( temp -> left -> content == "nil" )
        NewNode( re, "#t" ) ;
      else 
        NewNode( re, "nil" ) ;
    } // else

    return re ;
  } // Not()

  Node *And( Node *temp, int stage ) {
    Node *re = new Node ;
    Node *a = new Node ;
    bool isnil = false ;
    int non = 0 ;
    if ( stage == 1 ) {
      re = new Node ;
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content == "nil" ) {
          return a ;
        } // else if
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if 
        else if ( temp -> left -> content == "nil" ) {
          return temp -> left ;
        } // if
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = And( temp -> right, stage ) ;
    } // if
    else {
      re = new Node ;
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content == "nil" ) {
          return a ;
        } // else if

        re = a ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content == "nil" ) {
          return temp -> left ;
        } // else if

        re = temp -> left ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = And( temp -> right, stage ) ;
    } // else

    stage-- ;

    return re ;
  } // And()

  Node *Or( Node *temp, int stage ) {
    Node *re = new Node ;
    Node *a = new Node ;
    int non = 0 ;
    if ( stage == 1 ) {
      re = new Node ;
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content != "nil" ) {
          return a ;
        } // if
      } // if
      else { 
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content != "nil" ) {
          return temp -> left ;
        } // else if
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = And( temp -> right, stage ) ;
    } // if
    else {
      re = new Node ;
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content != "nil" ) {
          return a ;
        } // else if

        re = a ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content != "nil" ) {
          return temp -> left ;
        } // else if

        re = temp -> left ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = And( temp -> right, stage ) ;
    } // else

    return re ;
  } // Or()

  Node *Compare( Node *temp, int mode, int stage, bool &wrong ) {
    float aa = 0 ; 
    Node *re = new Node ;
    Node *a = new Node ; 
    int non = 0 ;
    if ( stage == 1 ) {
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          if ( mode == 0 ) {
            cout << "ERROR (> with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ;
          } // if
          else if ( mode == 1 ) {
            cout << "ERROR (< with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ;
          } // else if
          else if ( mode == 2 ) {
            cout << "ERROR (>= with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ;
          } // else if
          else if ( mode == 3 ) {
            cout << "ERROR (<= with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ;
          } // else if
          else if ( mode == 4 ) {
            cout << "ERROR (= with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ;
          } // else if
        } // else if

        mresult = ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          if ( mode == 0 )
            cout << "ERROR (> with incorrect argument type) : " << temp -> left -> content << endl ;
          else if ( mode == 1 )
            cout << "ERROR (< with incorrect argument type) : " << temp -> left -> content << endl ;
          else if ( mode == 2 )
            cout << "ERROR (>= with incorrect argument type) : " << temp -> left -> content << endl ;
          else if ( mode == 3 )
            cout << "ERROR (<= with incorrect argument type) : " << temp -> left -> content << endl ;
          else if ( mode == 4 )
            cout << "ERROR (= with incorrect argument type) : " << temp -> left -> content << endl ;

          return NULL ;
        } // else if
        else
          mresult = ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Compare( temp -> right, mode, stage, wrong ) ;
    } // if
    else if ( mode == 0 ) { // > 
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (> with incorrect argument type) : "  ;
          Preorder( a, false, false, false, 0 ) ;


          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (> with incorrect argument type) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( mresult <= aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mresult = aa ;
        re = Compare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // else if
    else if ( mode == 1 ) { // < 
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (< with incorrect argument type) : "  ;
          Preorder( a, false, false, false, 0 ) ;


          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (< with incorrect argument type) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( mresult >= aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mresult = aa ;
        re = Compare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // else if
    else if ( mode == 2 ) { // >=
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (>= with incorrect argument type) : "  ;
          Preorder( a, false, false, false, 0 ) ;


          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (>= with incorrect argument type) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( mresult < aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mresult = aa ;
        re = Compare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // else if
    else if ( mode == 3 ) { // <=
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (<= with incorrect argument type) : "  ;
          Preorder( a, false, false, false, 0 ) ;


          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (<= with incorrect argument type) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( mresult > aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mresult = aa ;
        re = Compare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // else if
    else if ( mode == 4 ) { // =
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( !IsInt( a -> content ) && !IsReal( a -> content ) ) {
          cout << "ERROR (= with incorrect argument type) : "  ;
          Preorder( a, false, false, false, 0 ) ;


          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( a -> content ) ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( !IsInt( temp -> left -> content ) && !IsReal( temp -> left -> content ) ) {
          cout << "ERROR (= with incorrect argument type) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // else if
        else
          aa = ConvertIntoNum( temp -> left -> content ) ;
      } // else

      if ( mresult != aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mresult = aa ;
        re = Compare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // if

    stage-- ;
    if ( stage == 1 && re != NULL ) {
      if ( wrong ) 
        NewNode( re, "nil" ) ;
      else
        NewNode( re, "#t" ) ;
    } // if

    return re ;
  } // Compare()

  Node *StringAppend( Node *temp, int stage, string &stringappend ) {
    int non = 0 ;
    Node *re = new Node ;
    if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
      Node *a = new Node ;
      if ( temp -> left -> content == "{DEFINE}" )
        a = EvalExp( temp -> left, NULL, false ) ;
      else
        a = EvalExp( temp -> left, NULL, true ) ;

      if ( a == NULL )
        return NULL ;
      else if ( a -> content != "" && a -> content.at( 0 ) != '\"' ) {
        cout << "ERROR (string-append with incorrect argument type) : " << 
                a -> content << endl ; 

        return NULL ;
      } // else if
      else {
        for ( int i = 1 ; i < a -> content.size() - 1 ; i++ ) {
          stringappend = stringappend + a -> content.at( i ) ;
        } // for
      } // else
    } // if
    else {
      if ( !InNodeListNotFix( temp -> left -> content, non ) &&
           !InFunctionList( ReturnContent( temp ) ) ) {
        cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

        return NULL ;
      } // if
      else if ( temp -> left -> content != "" && temp -> left -> content.at( 0 ) != '\"' ) {
        cout << "ERROR (string-append with incorrect argument type) : " << 
                temp -> left -> content << endl ; 

        return NULL ;
      } // else if
      else {
        for ( int i = 1 ; i < temp -> left -> content.size() - 1 ; i++ ) {
          stringappend = stringappend + temp -> left -> content.at( i ) ;
        } // for
      } // else
    } // else

    stage++ ;
    if ( temp -> right != NULL && temp -> right -> content != "nil" )
      re = StringAppend( temp -> right, stage, stringappend ) ;
    stage-- ;

    if ( stage == 1 && re != NULL ) {
      stringappend = stringappend + '\"' ;
      re = new Node ;
      NewNode( re, stringappend ) ;
    } // if

    return re ;
  } // StringAppend()

  Node *StringCompare( Node *temp, int mode, int stage, bool &wrong ) {
    string aa = "" ; 
    int non = 0 ;
    Node *re = new Node ;
    Node *a = new Node ; 
    if ( stage == 1 ) {
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content != "" && a -> content.at( 0 ) != '\"' ) {
          if ( mode == 0 ) {
            cout << "ERROR (string>? with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ; 
          } // if
          else if ( mode == 1 ) {
            cout << "ERROR (string<? with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ; 
          } // else if
          else if ( mode == 2 ) {
            cout << "ERROR (string=? with incorrect argument type) : " ;
            Preorder( a, false, false, false, 0 ) ; 
          } // else if

          return NULL ;
        } // else if
        else 
          mstringappend = a -> content  ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content != "" && temp -> left -> content.at( 0 ) != '\"' ) {
          if ( mode == 0 )
            cout << "ERROR (string>? with incorrect argument type) : " << 
                     temp -> left -> content << endl ; 
          else if ( mode == 1 )
            cout << "ERROR (string<? with incorrect argument type) : " << 
                     temp -> left -> content << endl ; 
          else if ( mode == 2 )
            cout << "ERROR (string=? with incorrect argument type) : " << 
                     temp -> left -> content << endl ;

          return NULL ;
        } // else if
        else 
          mstringappend = temp -> left -> content ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = StringCompare( temp -> right, mode, stage, wrong ) ;
    } // if
    else if ( mode == 0 ) { // > 
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content != "" && a -> content.at( 0 ) != '\"' ) {
          cout << "ERROR (string>? with incorrect argument type) : " ;
          Preorder( a, false, false, false, 0 ) ;

          return NULL ;
        } // else if
        else
          aa = a -> content ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content != "" && temp -> left -> content.at( 0 ) != '\"' ) {
          cout << "ERROR (string>? with incorrect argument type) : " << 
                  temp -> left -> content << endl ;

          return NULL ; 
        } // else if
        else
          aa = temp -> left -> content  ;
      } // else

      if ( mstringappend <= aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mstringappend = aa ;
        re = StringCompare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // else if
    else if ( mode == 1 ) { // < 
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content != "" && a -> content.at( 0 ) != '\"' ) {
          cout << "ERROR (string<? with incorrect argument type) : " ;
          Preorder( a, false, false, false, 0 ) ;

          return NULL ;
        } // else if
        else
          aa = a -> content ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content != "" && temp -> left -> content.at( 0 ) != '\"' ) {
          cout << "ERROR (string<? with incorrect argument type) : " << 
                  temp -> left -> content << endl ;

          return NULL ; 
        } // else if
        else
          aa = temp -> left -> content  ;
      } // else

      if ( mstringappend >= aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mstringappend = aa ;
        re = StringCompare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // else if
    else if ( mode == 2 ) { // =
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else if ( a -> content != "" && a -> content.at( 0 ) != '\"' ) {
          cout << "ERROR (string=? with incorrect argument type) : " ;
          Preorder( a, false, false, false, 0 ) ;

          return NULL ;
        } // else if
        else
          aa = a -> content ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content != "" && temp -> left -> content.at( 0 ) != '\"' ) {
          cout << "ERROR (string=? with incorrect argument type) : " << 
                  temp -> left -> content << endl ;

          return NULL ; 
        } // else if
        else
          aa = temp -> left -> content  ;
      } // else

      if ( mstringappend != aa ) {
        wrong = true ;
        // return re ; 
      } // if

      if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
        mstringappend = aa ;
        re = StringCompare( temp -> right, mode, stage, wrong ) ;
      } // if
    } // else if

    stage-- ;
    if ( stage == 1 && re != NULL ) {
      if ( wrong ) 
        NewNode( re, "nil" ) ;
      else
        NewNode( re, "#t" ) ;
    } // if

    return re ;
  } // StringCompare()

  Node *Eqv( Node *temp, int stage, Node *eqv_first, Node *eqv_second, 
             string &eqvstr_first, string &eqvstr_second ) {
    Node *re = new Node ;
    Node *a = new Node ; 
    int non = 0 ;
    if ( stage == 1 ) {
      stage++ ;
      eqv_first = new Node ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          eqv_first = EvalExp( temp -> left, NULL, false ) ;
        else
          eqv_first = EvalExp( temp -> left, NULL, true ) ;

        if ( eqv_first == NULL )
          return NULL ;
        else 
          eqvstr_first = eqv_first -> content ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else {
          eqv_first = temp -> left ;
          eqvstr_first = temp -> left -> content ;
        } // else
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Eqv( temp -> right, stage, eqv_first, eqv_second, eqvstr_first, eqvstr_second ) ;
    } // if
    else if ( stage == 2 ) {
      stage++ ;
      eqv_second = new Node ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          eqv_second = EvalExp( temp -> left, NULL, false ) ;
        else
          eqv_second = EvalExp( temp -> left, NULL, true ) ;

        if ( eqv_second == NULL )
          return NULL ;
        else
          eqvstr_second = eqv_second -> content ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else {
          eqv_second = temp -> left ;
          eqvstr_second = temp -> left -> content ;
        } // else
      } // else

      if ( eqv_first == eqv_second || ( eqvstr_first != "" && eqvstr_first != "{DEFINE}" 
                                        && eqvstr_first == eqvstr_second
                                        && eqvstr_first .at( 0 ) != '\"'  ) ) {
        re = new Node ;
        NewNode( re, "#t" ) ;
      } // if
    } // else if

    stage-- ;
    if ( stage == 1 && re != NULL ) {
      if ( re -> content != "#t" ) {
        re = new Node ;
        NewNode( re, "nil" ) ;
      } // if
    } // if

    return re ;
  } // Eqv()

  Node *Equal( Node *temp, int stage, Node *equal_first, Node *equal_second, 
               string &equalstr_first, string &equalstr_second ) {
    int non = 0 ;
    Node *re = new Node ;
    Node *a = new Node ; 
    if ( stage == 1 ) {
      stage++ ;
      equal_first = new Node ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          equal_first = EvalExp( temp -> left, NULL, false ) ;
        else
          equal_first = EvalExp( temp -> left, NULL, true ) ;

        if ( equal_first == NULL )
          return NULL ;
        else
          equalstr_first = a -> content ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else {
          equal_first = temp -> left ;
          equalstr_first = temp -> left -> content ;
        } // else
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = Equal( temp -> right, stage, equal_first, equal_second, equalstr_first, equalstr_second ) ;
    } // if
    else if ( stage == 2 ) {
      stage++ ;
      equal_second = new Node ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          equal_second = EvalExp( temp -> left, NULL, false ) ;
        else
          equal_second = EvalExp( temp -> left, NULL, true ) ;

        if ( equal_second == NULL )
          return NULL ;
        else
          equalstr_second = a -> content ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else {
          equal_second = temp -> left ;
          equalstr_second = temp -> left -> content ;
        } // else
      } // else

      if ( HaveSame( equal_first, equal_second ) || 
           ( equalstr_first != "" && equalstr_first == equalstr_second ) ) {
        re = new Node ;
        NewNode( re, "#t" ) ;
      } // if
    } // else if

    stage-- ;
    if ( stage == 1 && re != NULL ) {
      if ( re -> content != "#t" ) {
        re = new Node ;
        NewNode( re, "nil" ) ;
      } // if
    } // if

    return re ;    
  } // Equal()

  Node *If( Node *temp, int stage, bool condition ) {
    Node *re = new Node ;
    Node *a = new Node ;
    int non = 0 ;
    bool torf ;
    if ( stage == 1 ) {
      stage++ ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ; 
        else if ( a -> content != "nil" )
          torf = true ;
        else
          torf = false ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else if ( temp -> left -> content != "nil" )
          torf = true ;
        else 
          torf = false ;
      } // else

      if ( temp -> right != NULL && temp -> right -> content != "nil" )
        re = If( temp -> right, stage, torf ) ;
      
    } // if
    else if ( stage == 2 && condition ) {
      stage++ ;
      re = new Node ; 
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          re = EvalExp( temp -> left, NULL, false ) ;
        else
          re = EvalExp( temp -> left, NULL, true ) ;

        if ( re == NULL )
          return NULL ;
        else
          return re ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else {
          re = temp -> left ;
          return re ;
        } // else
      } // else
    } // else if
    else if ( stage == 3 && !condition ) {
      stage++ ;
      re = new Node ;
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          re = EvalExp( temp -> left, NULL, false ) ;
        else
          re = EvalExp( temp -> left, NULL, true ) ;

        if ( re == NULL )
          return NULL ;
        else
          return re ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else {
          re = temp -> left ;
          return re ;
        } // else
      } // else
    } // else if
    else {
      stage++ ;
      if ( stage == 3 && !condition && temp -> right == NULL ) {
        cout << "ERROR (no return value) : " ;
        re = new Node ;
        NewNode( re, "NoRE" ) ;
        return re ; // ERROR : While running code can find this error.
      } // if
      else
        re = If( temp -> right, stage, condition ) ;
    } // else

    stage--;
    return re ;
    
  } // If()

  Node *Cond( Node *temp ) {
    Node *re = new Node ;
    Node *a = new Node ;
    Node *ans = new Node ;
    Node *preprocess = new Node ;
    int non = 0 ;
    preprocess = temp -> left ;
    if ( preprocess -> right == NULL || ( preprocess -> right != NULL &&
                                          preprocess -> right -> content == "nil" ) ||
         IsNonList( preprocess, false, true ) ) {
      re = new Node ;
      NewNode( re, "ErrorFormat" ) ;
      return re ;
    } // if

    if ( preprocess -> left -> content == "" || preprocess -> left -> content == "{DEFINE}" ) {
      if ( preprocess -> left -> content == "{DEFINE}" )
        a = EvalExp( preprocess -> left, NULL, false ) ;
      else
        a = EvalExp( preprocess -> left, NULL, true ) ;

      if ( a == NULL ) {
        return NULL ;
      } // if
      else if ( a -> content != "nil" ) {
        a = new Node ;
        a = preprocess -> right ;
        while ( a != NULL && a -> content != "nil" ) {
          ans = new Node ;
          if ( a -> left -> content == "" || a -> left -> content == "{DEFINE}" ) {
            if ( a -> left -> content == "{DEFINE}" )
              ans = EvalExp( a -> left, NULL, false ) ;
            else 
              ans = EvalExp( a -> left, NULL, true ) ;
          } // if 
          else
            ans = a -> left ;

          if ( ans == NULL )
            return NULL ;

          a = a -> right ;
        } // while

        return ans ;
      } // else if
    } // if 
    else {
      if ( preprocess -> left -> content == "else" && ( temp -> right == NULL ||
                                                        ( temp -> right != NULL && 
                                                          temp -> right -> content == "nil" ) ) ) {
        a = new Node ;
        a = preprocess -> right ;
        while ( a != NULL && a -> content != "nil" ) {
          ans = new Node ;
          if ( a -> left -> content == "" || a -> left -> content == "{DEFINE}" ) {
            if ( a -> left -> content == "{DEFINE}" )
              ans = EvalExp( a -> left, NULL, false ) ;
            else 
              ans = EvalExp( a -> left, NULL, true ) ;
          } // if 
          else
            ans = a -> left ;

          if ( ans == NULL )
            return NULL ;

          a = a -> right ;
        } // while
          
        return ans ;
      } // if
      else if ( !InNodeListNotFix( preprocess -> left -> content, non ) &&
                !InFunctionList( ReturnContent( preprocess ) ) ) {
        cout << "ERROR (unbound symbol) : " << preprocess -> left -> content << endl ; 

        return NULL ;
      } // else if
      else if ( preprocess -> left -> content != "nil" ) {
        a = new Node ;
        a = preprocess -> right ;
        while ( a != NULL && a -> content != "nil" ) {
          ans = new Node ;
          if ( a -> left -> content == "" || a -> left -> content == "{DEFINE}" ) {
            if ( a -> left -> content == "{DEFINE}" )
              ans = EvalExp( a -> left, NULL, false ) ;
            else 
              ans = EvalExp( a -> left, NULL, true ) ;
          } // if 
          else
            ans = a -> left ;

          if ( ans == NULL )
            return NULL ;

          a = a -> right ;
        } // while

        return ans ;
      } // else if
    } // else

    if ( temp -> right != NULL && temp -> right -> content != "nil" )
      re = Cond( temp -> right ) ;
    else if ( preprocess -> left -> content != "else" && ( temp -> right == NULL ||
                                                           ( temp -> right != NULL && 
                                                             temp -> right -> content == "nil" ) ) ) {
      re = new Node ;
      NewNode( re, "NORE" ) ;
      return re ; // ERROR : WHILE running can find.
    } // else if

    return re ;
  } // Cond()

  Node *Begin( Node *temp ) {
    Node *re = new Node ;
    Node *a = new Node ;
    int non = 0 ;
    if ( temp != NULL && temp -> content != "nil" ) {
      if ( temp -> left -> content == "" || temp -> left -> content == "{DEFINE}" ) {
        if ( temp -> left -> content == "{DEFINE}" )
          a = EvalExp( temp -> left, NULL, false ) ;
        else
          a = EvalExp( temp -> left, NULL, true ) ;

        if ( a == NULL )
          return NULL ;
        else
          re = a ;
      } // if
      else {
        if ( !InNodeListNotFix( temp -> left -> content, non ) &&
             !InFunctionList( ReturnContent( temp ) ) ) {
          cout << "ERROR (unbound symbol) : " << temp -> left -> content << endl ; 

          return NULL ;
        } // if
        else
          re = temp -> left ;
      } // else
    } // if

    if ( temp -> right != NULL && temp -> right -> content != "nil" ) {
      re = Begin( temp -> right ) ;
    } // if

    return re ;
  } // Begin()
 
  Node *EvalExp( Node *temp, Node *non, bool re ) {
    // cout << mnodelist.size() << " --NODE-- " << mnotElist.size() << " --NOTE-- " << endl ;
    int index = -1 ;
    bool eval = false ; 
    if ( IsNonList( temp, false, true ) ) {  // Non list
      cout << "ERROR (non-list) : " ;
      FixBack( temp ) ; 
      Preorder( temp, false, false, false, 0 ) ;
      return NULL ;
    } // if
 
    if ( temp -> left != NULL && ReturnContent( temp ) == "" && re ) {
      eval = true ;
      Node *a = EvalExp( temp -> left, NULL, true ) ;
      if ( a == NULL )
        return NULL ;
      temp -> left = new Node ;
      temp -> left = a ;
    } // if

    // cout << "-----------------begin--------------------" << re << endl ;
    // Preorder( temp, false, false, false, 0 ) ;
    // cout << "------------------end--------------------" << endl ;

    if ( temp -> content == "{DEFINE}" ) {
      return temp -> left ;
    } // if
    else if ( temp -> content != ""  ) {
      return temp ; 
    } // else if
    else if ( ReturnContent( temp ) == "define" || ReturnContent( temp ) == "#<procedure define>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure define>" ) ) {
      if ( ( ReturnContent( mtnode ) != "define" &&
             ReturnContent( mtnode ) != "#<procedure define>" &&
             ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
               ReturnContent( temp -> left ) != "#<procedure define>" ) ) || mstage != 1 ) {
        cout << "ERROR (level of DEFINE)" << endl ;
        return NULL ;
      } // if
      else if ( !FormatType( temp, 2, 0, 3 ) ) {
        cout << "ERROR (DEFINE format) : " ;
        FixBack( temp ) ;
        Preorder( temp, false, false, false, 0 ) ;
        return NULL ;
      } // else if
      else {
        mstage++ ;
        Node *a = new Node ;
        a = Define( temp -> right, non, 1 ) ;
        if ( a != NULL && a -> content == "FormatError" ) {
          FixBack( temp ) ;
          Preorder( temp, false, false, false, 0 ) ;
        } // if
        else
          return a ;
      } // else
    } // else if
    else if ( ReturnContent( temp ) == "clean-environment" ||
              ReturnContent( temp ) == "#<procedure clean-environment>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure clean-environment>" ) ) {
      if ( ( ReturnContent( mtnode ) != "clean-environment" && 
             ReturnContent( mtnode ) != "#<procedure clean-environment>" &&
             ( mtnode -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
               ReturnContent( mtnode -> left ) != "#<procedure clean-environment>" ) ) || mstage != 1 ) {
        cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl ;
        return NULL ;
      } // if
      else if ( !ArgumentNum( temp, 0, 0, "clean-environment", 3 ) ) {
        return NULL ;
      } // else if
      else {
        mstage++ ;
        mdefinelist.clear() ;
        mdefinefunctionlist.clear() ;
        Node *a = new Node ;
        NewNode( a, "environment cleaned" ) ;
        return a ;
      } // else
    } // else if
    else if ( ReturnContent( temp ) == "cons" || ReturnContent( temp ) == "#<procedure cons>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure cons>" ) ) {
      if ( !ArgumentNum( temp, 2, 0, "cons", 3 ) ) {
        return NULL ;
      } // if
      else {
        mstage++ ;
        Node *dummy_root = new Node ;
        NewNode( dummy_root, "" ) ;
        return Cons( temp -> right, 1, dummy_root ) ;
      } // else  
    } // else if
    else if ( ReturnContent( temp ) == "list" || ReturnContent( temp ) == "#<procedure list>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure list>" ) ) {
      mstage++ ;
      if ( temp -> right == NULL ) {
        Node *a = new Node ;
        NewNode( a, "nil" ) ;
        return a ;
      } // if 

      return List( temp -> right, 1 ) ;
    } // else if
    else if ( ReturnContent( temp ) == "\'" || ReturnContent( temp ) == "quote" ||
              ReturnContent( temp ) == "#<procedure quote>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure quote>" ) ) {
      if ( ReturnContent( temp ) == "\'" )
        return temp -> right ;
      else {
        if ( !ArgumentNum( temp, 1, 0, "quote", 3 ) )
          return NULL ;
        else
          return temp -> right -> left ;
      } // else
    } // else if
    else if ( ReturnContent( temp ) == "car" || ReturnContent( temp ) == "#<procedure car>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure car>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "car", 3 ) ) {
        return NULL ;
      } // if
      else 
        return Car( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "cdr" || ReturnContent( temp ) == "#<procedure cdr>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure cdr>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "cdr", 3 ) ) {
        return NULL ;
      } // if
      else
        return Cdr( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "atom?" || ReturnContent( temp ) == "#<procedure atom?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure atom?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "atom", 3 ) ) {
        return NULL ;
      } // if
      else
        return Atom( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "pair?" || ReturnContent( temp ) == "#<procedure pair?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure pair?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "pair?", 3 ) ) {
        return NULL ;
      } // if
      else
        return Pair( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "list?" || ReturnContent( temp ) == "#<procedure list?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure list?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "list?", 3 ) ) {
        return NULL ;
      } // if
      else
        return IsList( temp -> right, 1 ) ;
    } // else if
    else if ( ReturnContent( temp ) == "null?" || ReturnContent( temp ) == "#<procedure null?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure null?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "null?", 3 ) ) {
        return NULL ;
      } // if
      else
        return Null( temp -> right ) ;
    } // else if 
    else if ( ReturnContent( temp ) == "integer?" || ReturnContent( temp ) == "#<procedure integer?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure integer?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "integer?", 3 ) ) {
        return NULL ;
      } // if
      else
        return Int( temp -> right ) ;
    } // else if 
    else if ( ReturnContent( temp ) == "real?" || ReturnContent( temp ) == "#<procedure real?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure real?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "real?", 3 ) ) {
        return NULL ;
      } // if
      else
        return Real( temp -> right ) ;
    } // else if 
    else if ( ReturnContent( temp ) == "number?" || ReturnContent( temp ) == "#<procedure number?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure numbr?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "number?", 3 ) ) {
        return NULL ;
      } // if
      else
        return IsNumber( temp -> right ) ;
    } // else if 
    else if ( ReturnContent( temp ) == "string?" || ReturnContent( temp ) == "#<procedure string?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure string?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "string?", 3 ) ) {
        return NULL ;
      } // if
      else
        return String( temp -> right ) ;
    } // else if 
    else if ( ReturnContent( temp ) == "boolean?" || ReturnContent( temp ) == "#<procedure boolean?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure boolean?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "boolean?", 3 ) ) {
        return NULL ;
      } // if 
      else
        return Boolean( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "symbol?" || ReturnContent( temp ) == "#<procedure symbol?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure symbol?>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "sumbol?", 3 ) ) {
        return NULL ;
      } // if
      else
        return Symbol( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "+" || ReturnContent( temp ) == "-" ||
              ReturnContent( temp ) == "*" || ReturnContent( temp ) == "/" ||
              ReturnContent( temp ) == "#<procedure +>" || ReturnContent( temp ) == "#<procedure ->" ||
              ReturnContent( temp ) == "#<procedure *>" || ReturnContent( temp ) == "#<procedure />" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure +>" ) ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure ->" ) ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure *>" ) ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure />" ) )  {
      float result = 0 ;
      mstage++ ;
      if ( ReturnContent( temp ) == "+" || ReturnContent( temp ) == "#<procedure +>" ||
           ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
             ReturnContent( temp -> left ) == "#<procedure +>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "+", 2 ) ) {
          return NULL ;
        } // if
        else
          return Arithmetic( temp -> right, 1, 0, result ) ;
      } // if
      else if ( ReturnContent( temp ) == "-" || ReturnContent( temp ) == "#<procedure ->" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure ->" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "-", 2 ) ) {
          return NULL ;
        } // if
        else
          return Arithmetic( temp -> right, 1, 1, result ) ;
      } // else if
      else if ( ReturnContent( temp ) == "*" || ReturnContent( temp ) == "#<procedure *>" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure *>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "*", 2 ) ) {
          return NULL ;
        } // if
        else
          return Arithmetic( temp -> right, 1, 2, result ) ;
      } // else if
      else if ( ReturnContent( temp ) == "/" || ReturnContent( temp ) == "#<procedure />" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure />" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "/", 2 ) ) {
          return NULL ;
        } // if
        else
          return Arithmetic( temp -> right, 1, 3, result ) ;
      } // else if
    } // else if
    else if ( ReturnContent( temp ) == "not" || ReturnContent( temp ) == "#<procedure not>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure not>" ) ) {
      if ( !ArgumentNum( temp, 1, 0, "not", 3 ) ) {
        return NULL ;
      } // if
      else
        return Not( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "and" || ReturnContent( temp ) == "#<procedure and>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure and>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 2, 0, "and", 2 ) ) {
        return NULL ;
      } // if
      else
        return And( temp -> right, 1 ) ;
    } // else if
    else if ( ReturnContent( temp ) == "or" || ReturnContent( temp ) == "#<procedure or>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure or>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 2, 0, "or", 2 ) ) {
        return NULL ;
      } // if
      else
        return Or( temp -> right, 1 ) ;
    } // else if
    else if ( ReturnContent( temp ) == ">" || ReturnContent( temp ) == "<" ||
              ReturnContent( temp ) == ">=" || ReturnContent( temp ) == "<=" ||
              ReturnContent( temp ) == "=" || ReturnContent( temp ) == "#<procedure >>" ||
              ReturnContent( temp ) == "#<procedure <>" || ReturnContent( temp ) == "#<procedure >=>" ||
              ReturnContent( temp ) == "#<procedure <=>" || ReturnContent( temp ) == "#<procedure =>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure >>" ) ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure <>" ) || 
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure >=>" ) ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure <=>" ) ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure =>" ) ) {
      mstage++ ;
      bool wrong = false ;
      if ( ReturnContent( temp ) == ">" || ReturnContent( temp ) == "#<procedure >>" ||
           ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
             ReturnContent( temp -> left ) == "#<procedure >>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, ">", 2 ) ) {
          return NULL ;
        } // if
        else
          return Compare( temp -> right, 0, 1, wrong ) ;
      } // if
      else if ( ReturnContent( temp ) == "<" || ReturnContent( temp ) == "#<procedure <>" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure <>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "<", 2 ) ) {
          return NULL ;
        } // if
        else
          return Compare( temp -> right, 1, 1, wrong ) ;
      } // else if
      else if ( ReturnContent( temp ) == ">=" || ReturnContent( temp ) == "#<procedure >=>" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure >=>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, ">=", 2 ) ) {
          return NULL ;
        } // if
        else
          return Compare( temp -> right, 2, 1, wrong ) ;
      } // else if
      else if ( ReturnContent( temp ) == "<=" || ReturnContent( temp ) == "#<procedure <=>" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure <=>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "<=", 2 ) ) {
          return NULL ;
        } // if
        else
          return Compare( temp -> right, 3, 1, wrong ) ;
      } // else if
      else if ( ReturnContent( temp ) == "=" || ReturnContent( temp ) == "#<procedure =>" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                  ReturnContent( temp -> left ) == "#<procedure =>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "=", 2 ) ) {
          return NULL ;
        } // if
        else
          return Compare( temp -> right, 4, 1, wrong ) ;
      } // else if
    } // else if
    else if ( ReturnContent( temp ) == "string-append" || 
              ReturnContent( temp ) == "#<procedure string-append>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" && 
                ReturnContent( temp -> left ) == "#<procedure string-append>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 2, 0, "string-append", 2 ) ) {
        return NULL ;
      } // if
      else {
        string stringappend = "" ;
        stringappend = stringappend + '\"' ;
        return StringAppend( temp -> right, 1, stringappend ) ; 
      } // else 
    } // else if
    else if ( ReturnContent( temp ) == "string>?" || ReturnContent( temp ) == "string<?" || 
              ReturnContent( temp ) == "string=?" || ReturnContent( temp ) == "#<procedure string>?>" ||
              ReturnContent( temp ) == "#<procedure string<?>" ||
              ReturnContent( temp ) == "#<procedure string=?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure string>?>" ) || 
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure string<?>" ) ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure string=?>" ) ) {
      mstage++ ;
      bool wrong = false ;
      if ( ReturnContent( temp ) == "string>?" || ReturnContent( temp ) == "#<procedure string>?>" ||
           ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
             ReturnContent( temp -> left ) == "#<procedure string>?>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "string>?", 2 ) ) {
          return NULL ;
        } // if
        else
          return StringCompare( temp -> right, 0, 1, wrong ) ;
      } // if
      else if ( ReturnContent( temp ) == "string<?" || ReturnContent( temp ) == "#<procedure string<?>" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure string<?>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "string<?", 2 ) ) {
          return NULL ;
        } // if
        else
          return StringCompare( temp -> right, 1, 1, wrong ) ;
      } // else if
      else if ( ReturnContent( temp ) == "string=?" || ReturnContent( temp ) == "#<procedure string=?>" ||
                ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                  ReturnContent( temp -> left ) == "#<procedure string=?>" ) ) {
        if ( !ArgumentNum( temp, 2, 0, "string=?", 2 ) ) {
          return NULL ;
        } // if
        else
          return StringCompare( temp -> right, 2, 1, wrong ) ;
      } // else if
    } // else if 
    else if ( ReturnContent( temp ) == "eqv?" || ReturnContent( temp ) == "#<procedure eqv?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure eqv?>" ) ) {
      mstage++ ;
      Node *first = new Node ;
      Node *second = new Node ;
      string ffirst = "", ssecond = "" ;
      if ( !ArgumentNum( temp, 2, 0, "eqv?", 3 ) ) {
        return NULL ;
      } // if
      else
        return Eqv( temp -> right, 1, first, second, ffirst, ssecond ) ;
    } // else if
    else if ( ReturnContent( temp ) == "equal?" || ReturnContent( temp ) == "#<procedure equal?>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure equal?>" ) ) {
      mstage++ ;
      Node *first = new Node ;
      Node *second = new Node ;
      string ffirst = "" ;
      string ssecond = "" ;
      if ( !ArgumentNum( temp, 2, 0, "equal?", 3 ) ) {
        return NULL ;
      } // if
      else
        return Equal( temp -> right, 1, first, second, ffirst, ssecond ) ;
    } // else if
    else if ( ReturnContent( temp ) == "if" || ReturnContent( temp ) == "#<procedure if>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure if>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 2, 0, "if", 2 ) || !ArgumentNum( temp, 3, 0, "if", 1 ) ) {
        return NULL ;
      } // if
      else {
        Node *a = new Node ;
        a = If( temp -> right, 1, false ) ; 
        if ( a != NULL && a -> content == "NoRE" ) {
          FixBack( temp ) ;
          Preorder( temp, false, false, false, 0 ) ;
          return NULL ;
        } // if
        else
          return a ;
      } // else
    } // else if
    else if ( ReturnContent( temp ) == "cond" || ReturnContent( temp ) == "#<procedure cond>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure cond>" ) ) {
      mstage++ ;
      if ( !FormatType( temp, 1, 0, 2 ) || FormatCond( temp -> right ) ) {
        cout << "ERROR (COND format) : " ;
        FixBack( temp ) ;
        Preorder( temp, false, false, false, 0 ) ;
        return NULL ;
      } // if
      else {
        Node *check = new Node ;
        Node *a = new Node ;
        check = ReturnNodeInNodeList( "else" ) ;
        if ( check == NULL )
          check = NULL ;
        else  
          check = check -> left ;
        a = FixElse( temp -> right, check ) ;
        if ( a == NULL )
          return NULL ;
        // Preorder( temp -> right, false, false, false, 0 ) ;
        a = new Node ;
        a = Cond( temp -> right ) ;
        if ( a != NULL && a -> content == "ErrorFormat" ) {
          cout << "ERROR (COND format) : " ;
          FixBack( temp ) ;
          Preorder( temp, false, false, false, 0 ) ;
          return NULL ;
        } // if
        else if ( a != NULL && a -> content == "NORE" ) {
          cout << "ERROR (no return value) : " ;
          FixBack( temp ) ;
          Preorder( temp, false, false, false, 0 ) ;
          return NULL ;
        } // else if
        else 
          return a ;
      } // else
    } // else if
    else if ( ReturnContent( temp ) == "begin" || ReturnContent( temp ) == "#<procedure begin>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure begin>" ) ) {
      mstage++ ;
      if ( !ArgumentNum( temp, 1, 0, "begin", 2 ) ) {
        return NULL ;
      } // if
      else
        return Begin( temp -> right ) ;
    } // else if
    else if ( ReturnContent( temp ) == "exit" || ReturnContent( temp ) == "#<procedure exit>" ||
              ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
                ReturnContent( temp -> left ) == "#<procedure exit>" ) ) {
      if ( ( ReturnContent( mtnode ) != "exit" && ReturnContent( mtnode ) != "#<procedure exit>" &&
             ( temp -> left -> left != NULL && temp -> left -> content == "{DEFINE}" &&
               ReturnContent( temp -> left ) != "#<procedure exit>" ) ) || mstage != 1  ) {
        cout << "ERROR (level of EXIT)" << endl ;
        return NULL ;
      } // if
      else if ( !ArgumentNum( temp, 0, 0, "exit", 3 ) )
        return NULL ;
      else {
        mstage++ ;
        mover = true ;
        return NULL ;
      } // else
    } // else if
    else if ( !eval && ( temp -> left != NULL && !InNodeListNotFix( ReturnContent( temp ), index ) ) &&
              ( temp -> left != NULL && !InDefine( temp -> left, index ) ) ) {
      cout << "ERROR (unbound symbol) : " << ReturnContent( temp ) << endl ; 

      return NULL ;
    } // else if
    else {
      if ( index == -1 ) {
        cout << "ERROR (attempt to apply non-function) : " ;
        Preorder( temp -> left, false, false, false, 0 ) ;
      } // if 
      else {
        Node *a = new Node ;
        a = mdefinelist.at( index ).node ;
        cout << "ERROR (attempt to apply non-function) : " ;
        Preorder( a, false, false, false, 0 ) ;
      } // else

      return NULL ;
    } // else

    return NULL ;
  } // EvalExp()

  void FixBack( Node *a ) {
    if ( a -> left != NULL && a -> left -> content == "{DEFINE}" ) {
      string name = SameNode( a -> left ) ;
      Node *temp = new Node ;
      NewNode( temp, name ) ;
      a -> left = new Node ;
      a -> left = temp ;
    } // if

    if ( a -> left != NULL )
      FixBack( a -> left ) ;

    if ( a -> right != NULL )
      FixBack( a -> right ) ;
  } // FixBack()

  string SameNode( Node *a ) {
    for ( int i = 0 ; i < mdefinelist.size() ; i++ ) {
      if ( HaveSame( a, mdefinelist.at( i ).node ) )
        return mdefinelist.at( i ).name ;
    } // for

    return "" ;
  } // SameNode()

  bool FormatCond( Node *temp ) {
    Node *preprocess = new Node ;
    preprocess = temp -> left ;
    if ( preprocess -> right == NULL || ( preprocess -> right != NULL &&
                                          preprocess -> right -> content == "nil" ) ||
         IsNonList( preprocess, false, true ) ) {
      return true ;
    } // if

    if ( temp -> right != NULL && temp -> right -> content != "nil" )
      return FormatCond( temp -> right ) ;

    return false ;
  } // FormatCond()

  bool IsDefine( string str ) {
    for ( int i = 0 ; i < mdefinefunctionlist.size() ; i++ ) {
      if ( str == mdefinefunctionlist.at( i ).name ) {
        if ( mdefinefunctionlist.at( i ).node -> left -> content ==
             "#<procedure define>" ) {
          return true ;
        } // if
      } // if
    } // for

    return false ;
  } // IsDefine()

  bool IsQuote( string str ) {
    for ( int i = 0 ; i < mdefinefunctionlist.size() ; i++ ) {
      if ( str == mdefinefunctionlist.at( i ).name ) {
        if ( mdefinefunctionlist.at( i ).node -> left -> content ==
             "#<procedure quote>" ) {
          return true ;
        } // if
      } // if
    } // for

    return false ;
  } // IsQuote()

  Node *FixElse( Node *temp, Node *check ) {
    Node *re = new Node ;
    Node *a = new Node ;
    Node *preprocess = new Node ;
    int non = 0 ;
    preprocess = temp -> left ;
    if ( preprocess -> left == NULL )
      return re ;

    if ( preprocess -> left -> content == "" || preprocess -> left -> content == "{DEFINE}" ) {
      if ( preprocess -> left -> content == "{DEFINE}" )
        a = EvalExp( preprocess -> left, NULL, false ) ; 
      else
        a = EvalExp( preprocess -> left, NULL, true ) ;
 
      if ( a == NULL )
        return NULL;
      else if ( ( a -> content == "else" || HaveSame( a, check ) ) && 
                ( temp -> right == NULL || temp -> right -> content == "nil" ) ) {
        preprocess -> left = new Node ;
        NewNode( preprocess -> left, "else" ) ;
      } // else if
    } // if
    else {
      if ( ( preprocess -> left -> content == "else" || HaveSame( preprocess -> left, check ) ) && 
           ( temp -> right == NULL || temp -> right -> content == "nil" ) ) {
        preprocess -> left = new Node ;
        NewNode( preprocess -> left, "else" ) ;
      } // if
    } // else

    if ( temp -> right != NULL && temp -> right -> content != "nil" )
      re = FixElse( temp -> right, check ) ;

    return re ;
  } // FixElse()

  bool HaveSame( Node *a, Node *b ) {
    int size = 0 ;
    vector<string> recorda, recordb ;
    PreorderSame( a, false, false, false, recorda ) ;
    PreorderSame( b, false, false, false, recordb ) ;

    if ( recorda.size() >= recordb.size() )
      size = recorda.size() ;
    else
      size = recordb.size() ;

    for ( int i = 0 ; i < size ; i++ ) {
      if ( i == recorda.size() && ( i < size || i == 0 ) )
        return false ;
      else if ( i == recordb.size() && ( i < size || i == 0 ) )
        return false ;
      else if ( recorda.at( i ) != recordb.at( i ) )
        return false ;
    } // for

    return true ;
  } // HaveSame()

  float ConvertIntoNum( string str ) {
    float a = 0 ;
    bool dot = false, neg = false ;
    int locbehinddot = 1 ;
    for ( int i = 0 ; i < str.size() ; i++ ) {
      if ( str.at( i ) == '-' )
        neg = true ;
      else if ( str.at( i ) == '+' )
        ;
      else if ( str.at( i ) == '.' ) {
        mfloat = true ;
        dot = true ;
      } // else if
      else if ( !dot ) {
        a = 10 * a + ( str.at( i ) - '0' ) ;
      } // else if 
      else if ( dot ) {
        a = a + ( str.at( i ) - '0' ) / pow( 10, locbehinddot ) ;
        locbehinddot++ ;
      } // else if
    } // for

    if ( neg )
      a = a * -1 ;

    return a ;
  } // ConvertIntoNum()

  void PrintS( Node *a ) {
    if ( a != NULL )
      Preorder( a, false, false, false, 0 ) ;
  } // PrintS()

  bool ReturnNodeList() {
    for ( int i = 0 ; i < mdefinelist.size() ; i++ ) {
      if ( mtoken == mdefinelist[i].name ) {
        Preorder( mdefinelist[i].node, false, false, false, 0 ) ;
        return true ;
      } // if
    } // for

    if ( !IsInt( mtoken ) && !IsReal( mtoken ) && mtoken.at( 0 ) != '\"' &&
         !IsFun( mtoken ) && !IsDefineFun( mtoken ) && mtoken != "nil" &&
         mtoken != "#t" &&  mtoken != "." ) {
      cout << "ERROR (unbound symbol) : " << mtoken << endl << endl ; 
      msymbolerror = true ;
    } // if

    if ( mread == '\n' || mread == ')' || mstringM )
      mx = 0 ;
    else {
      mx = 1 ;
    } // else 

    my = 1 ;

    return false ;
  } // ReturnNodeList()

  bool ReturnFunctionList() {
    for ( int i = 0 ; i < mfunctionlist.size() ; i++ ) {
      if ( mtoken == mfunctionlist[i].name ) {
        Preorder( mfunctionlist[i].node, false, false, false, 0 ) ;
        return true ;
      } // if
    } // for

    return false ;
  } // ReturnFunctionList()

  bool InNodeList( string str, Node *a ) {
    for ( int i = 0 ; i < mdefinelist.size() ; i++ ) {
      if ( str == mdefinelist[i].name ) {
        mdefinelist[i].node = a ;
        return true ;
      } // if
    } // for

    return false ;
  } // InNodeList()

  bool InNotEval( string str, Node *a ) {
    for ( int i = 0 ; i < mnotevallist.size() ; i++ ) {
      if ( str == mnotevallist[i].name ) {
        mnotevallist[i].node = a ;
        return true ;
      } // if
    } // for

    return false ;
  } // InNotEval() 

  bool FixDefineFunctionList( string str, Node *a ) {
    for ( int i = 0 ; i < mdefinefunctionlist.size() ; i++ ) {
      if ( str == mdefinefunctionlist[i].name ) {
        mdefinelist[i].node = a ;
        return true ;
      } // if
    } // for

    return false ;
  } // FixDefineFunctionList()

  bool InNodeListNotFix( string str, int &num ) {
    for ( int i = 0 ; i < mdefinelist.size() ; i++ ) {
      if ( str == mdefinelist[i].name ) {
        num = i ;
        return true ;
      } // if
    } // for

    num = -1 ;
    if ( IsInt( str ) || IsReal( str ) || ( str != "" && str.at( 0 ) == '\"' ) ||
         IsFun( str ) || str == "nil" || str == "#t" || str == "." ||
         ( str != "" && str.at( 0 ) == '\'' ) ) {
      return true ;
    } // if

    return false ;
  } // InNodeListNotFix()

  bool InDefine( Node *a, int &num ) {
    for ( int i = 0 ; i < mdefinelist.size() ; i++ ) {
      if ( a == mdefinelist[i].node ) {
        num = i ;
        return true ;
      } // if
    } // for

    num = -1 ;
    return false ;
  } // InDefine()

  Node *ReturnNodeInNodeList( string str ) {
    for ( int i = 0 ; i < mdefinelist.size() ; i++ ) {
      if ( str == mdefinelist[i].name ) {
        return mdefinelist[i].node ;
      } // if
    } // for

    return NULL ;
  } // ReturnNodeInNodeList()

  Node *ReturnNodeInFunctionList( string str ) {
    for ( int i = 0 ; i < mfunctionlist.size() ; i++ ) {
      if ( str == mfunctionlist[i].name ) {
        return mfunctionlist[i].node ;
      } // if
    } // for

    return NULL ;
  } // ReturnNodeInFunctionList()

  bool InFunctionList( string str ) {
    for ( int i = 0 ; i < mfunctionlist.size() ; i++ ) {
      if ( str == mfunctionlist[i].node -> content ) {
        return true ;
      } // if
    } // for

    return false ;
  } // InFunctionList()

  bool IsFun( string str ) {
    for ( int i = 0 ; i < mfunctionlist.size() ; i++ ) {
      if ( str == mfunctionlist[i].name ) {
        return true ;
      } // if
    } // for

    return false ;
  } // IsFun() 

  bool IsDefineFun( string str ) {
    for ( int i = 0 ; i < mdefinefunctionlist.size() ; i++ ) {
      if ( str == mdefinefunctionlist[i].name ) {
        return true ;
      } // if
    } // for

    return false ; 
  } // IsDefineFun() 

  // ---------------------------------------new--------------------------------------

  bool ArgumentNum( Node *a, int num, int count, string str, int mode ) {
    bool re = false ;
    if ( a != NULL && a -> right != NULL && a -> right -> content != "nil" ) {
      count++ ;
      re = ArgumentNum( a -> right, num, count, str, mode ) ;
    } // if
    else {
      if ( mode == 1 ) {  // > ERROR
        if ( count > num ) {
          cout << "ERROR (incorrect number of arguments) : " << str << endl ;
          return false ;
        } // if
      } // if
      else if ( mode == 2 ) {
        if ( count < num ) { // < ERROR
          cout << "ERROR (incorrect number of arguments) : " << str << endl ;
          return false ;
        } // if
      } // else if
      else if ( mode == 3 ) {
        if ( count != num ) { // != ERROR
          cout << "ERROR (incorrect number of arguments) : " << str << endl ;
          return false ;
        } // if
      } // else if

      return true ;
    } // else

    return re ;
  } // ArgumentNum()

  bool FormatType( Node *a, int num, int count, int mode ) {
    bool re = false ;
    if ( a != NULL && a -> right != NULL && a -> right -> content != "nil" ) {
      count++ ;
      re = FormatType( a -> right, num, count, mode ) ;
    } // if
    else {
      if ( mode == 1 ) {  // > ERROR
        if ( count > num ) {
          return false ;
        } // if
      } // if
      else if ( mode == 2 ) {
        if ( count < num ) { // < ERROR
          return false ;
        } // if
      } // else if
      else if ( mode == 3 ) {
        if ( count != num ) { // != ERROR
          return false ;
        } // if
      } // else if
      
      return true ;
    } // else

    return re ;
  } // FormatType()

  bool IsNonList( Node *a, bool allright, bool first ) {
    if ( a -> left != NULL && ( a -> left -> content == "\'" ) && first )
      allright = true ;

    if ( a -> right != NULL && a -> right -> content != "nil" && a -> right -> content != "" &&
         !allright )
      return true ;
    else if ( a -> right != NULL && a -> right -> content != "nil" )
      return IsNonList( a -> right, allright, false ) ;

    return false ;
  } // IsNonList()

  // bool 
  void ReadAndToken() {
    mx = 0 ;
    my = 0 ;
    mquotenum = 0 ;
    mnumofL = 0 ;
    mdot = 1 ;
    mnumofAtom = -1 ;
    mThisLineHasSth = false ;
    mnum = true ;
    mleftmode = false ;
    mdoterror = false ;
    mError = false ;
    mover = false ;
    mquotefirst = true ;
    mfirst = true ;
    mstringM = false ;
    mnoclosingquote = false ;
    mquote = false ;
    mconvert = false ;
    mdone = false ;
    mpass = false ;
    mfloat = false ;
    msymbolerror = false ;
    mquoteerror = false ;
    mrepeat = false ;
    while ( scanf( "%c", &mread ) != EOF && !mover ) {
      if ( mread != '\n' ) 
        mx++ ;

      if ( mread == '\"' && !mError ) {
        mtoken = mtoken + mread ;
        DoubleQuote() ;
        if ( mnoclosingquote ) {
          cout << "> " ;
          cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line " << 
          my << " Column " << mx << endl ;
          cout << endl ;
          if ( mleftmode )
            mleftmode = false ;
          my = 1 ;
          mError = true ;
          mThisLineHasSth = true ;
        } // if
        else 
          mstringM = true ;
      } // if
      else if ( mread == '\'' && !mError ) {
        mTypeAndLoctemp.type = "QUOTE" ; 
        mTypeAndLoctemp.locx = mx ;
        mTypeAndLoctemp.locy = my ;
        mOuterQuote.push_back( mTypeAndLoctemp ) ;
        mquotenum++ ;
        mtoken = mtoken + mread ;
        mquote = true ;
        mstringM = true ;
      } // else if
      else if ( ( mread == '+' || mread == '-' || mread == '.' || ( mread >= '0' && mread <= '9' ) )
                && mnum == true && mtoken == "" && !mError && !mleftmode ) { // deal with "." as well
        Number() ;
        if ( mdoterror ) {
          cout << "> " ;
          cout << "ERROR (unexpected token) : atom or '(' expected when token at Line " << my << " Column " 
               << mx << " is >>.<<" << endl ; 
          cout << endl ;
          mError = true ;
          mThisLineHasSth = true ;
        } // if
      } // else if
      else if ( mread == 't' && mtoken == "" && ( cin.peek() == '\n' 
                                                  || cin.peek() == ' ' || cin.peek() == '\t' 
                                                  || cin.peek() == '(' || cin.peek() == ')' 
                                                  || cin.peek() == ';' || cin.peek() == '\"' 
                                                  || cin.peek() == '\'' ) 
                && !mError ) {
        mtoken = mtoken + "#t" ;
        if ( cin.peek() == '\"' || cin.peek() == '\'' || cin.peek() == '(' || cin.peek() == ')' )
          mstringM = true ;
      } // else if
      else if ( mread == '#' && mtoken == "" && !mError ) {
        mtoken = mtoken + mread ;
        scanf( "%c", &mread ) ;
        mx++ ;
        if ( ( mread == 'f' || mread == 't' ) && ( cin.peek() == '\n' || cin.peek() == ' ' 
                                                   || cin.peek() == '\t' || cin.peek() == '('
                                                   || cin.peek() == ')' || cin.peek() == ';' 
                                                   || cin.peek() == '\"' || cin.peek() == '\'' ) 
             && !mError ) {
          mtoken = "" ;
          if ( mread == 'f' )
            mtoken = mtoken + "nil" ;
          else if ( mread == 't' )
            mtoken = mtoken + "#t" ;

          if ( cin.peek() == '\"' || cin.peek() == '\'' || cin.peek() == '(' || cin.peek() == ')' )
            mstringM = true ;
          mconvert = true ;
        } // if
        else if ( mread != ' ' && mread != '\t' && mread != '(' &&
                  mread != ')' && mread != '\'' && mread != '\"' && 
                  mread != '\n' && mread != ';' ) {
          mtoken = mtoken + mread ;

          if ( cin.peek() == '\"' || cin.peek() == '\'' || cin.peek() == '(' || cin.peek() == ')' )
            mstringM = true ;
        } // else if
      } // else if
      else if ( mread != ' ' && mread != '\t' && mread != '(' &&
                mread != ')' && mread != '\'' && mread != '\"' && 
                mread != '\n' && mread != ';' && !mError && !mstringM ) {
        mtoken = mtoken + mread ;
        if ( cin.peek() == '\"' || cin.peek() == '\'' || cin.peek() == '(' || cin.peek() == ')' )
          mstringM = true ;
      } // else if
      else if ( mread == '(' && !mError ) {
        if ( !mleftmode )
          mleftmode = true ;
        mnumofL++ ;

        if ( cin.peek() == ')' ) {
          scanf( "%c", &mread ) ;
          mnumofL-- ;
          if ( mtokenstring.size() == 0 ) {
            mleftmode = false ;
            mtoken = mtoken + "nil" ;
            mnumofL = 0 ;
            mdot = 1 ;
          } // if
          else {
            if ( mtoken != "" )
              mtokenstring.push_back( mtoken ) ;
            mtokenstring.push_back( "nil" ) ;
            mTypeAndLoctemp.type = "ATOM" ;
            mTypeAndLoctemp.locx = mx ;  
            mTypeAndLoctemp.locy = my ;
            merrorcheck.push_back( mTypeAndLoctemp ) ;
            mtoken = "" ;
          } // else

          mx++ ;
        } // if
        else {
          if ( mtoken != "" ) {
            mtokenstring.push_back( mtoken ) ;
            mtoken = "" ;
          } // if

          mtoken = mtoken + "(" ;      
        } // else
      } // else if
      else if ( mread == ')' && !mError ) {

        if ( mtoken != "" ) {
          mtokenstring.push_back( mtoken ) ;
          mtoken = "" ;
        } // if

        mtoken = mtoken + ")" ;

        if ( mtokenstring.size() == 1 && mtokenstring.at( 0 ) == "(" ) {
          mtoken = "" ; 
          mtoken = mtoken + "nil" ;
          mleftmode = false ;
          mdot = 1 ;
          mtokenstring.clear() ;
        } // else if
        else if ( mtokenstring.size() == 0 ) { // NEEDED TO CHECK----------------
          cout << "> " ;
          cout << "ERROR (unexpected token) : atom or '(' expected when token at Line " 
          << my << " Column " << mx << " is >>)<<" << endl ; 
          cout << endl ;
          mError = true ; 
          mleftmode = false ;
          mtoken = "" ; 
          mThisLineHasSth = true ;
        } // else if
      } // else if
      else if ( mread == ';' && !mError ) {
        while ( mread != '\n' && cin.peek() != EOF )
          scanf( "%c", &mread ) ;
        if ( !mleftmode ) {
          mx = 0 ;
          if ( !mThisLineHasSth )
            my++ ;
        } // if

        mcomment = true ;
      } // else if

      // -----------------------------------start-----------------------------------------

      if ( ( mread == '\n' || mread == ' ' || mread == '\t' || mread == '(' || mread == ')' )
           || mError || mstringM ) {
        if ( mtoken != "" && !mleftmode && !mError && mtoken != "\'" )
          cout << "> "  ;

        if ( !mquote && !mleftmode && mtoken != "" && 
             ( ReturnNodeList() || ReturnFunctionList() ) ) {
          if ( mread == '\n' || mread == ')' || mstringM )
            mx = 0 ;
          else {
            mx = 1 ;
          } // else 

          my = 1 ;

          cout << endl ;
        } // if
        else if ( mleftmode ) {
          if ( mtoken != "" ) {
            if ( mtoken == ")" )
              mnumofL-- ;

            if ( mtoken == ")" && mtokenstring.at( mtokenstring.size() - 1 ) == "(" ) {
              mtokenstring.pop_back() ;
              mtoken = "nil" ;
              mTypeAndLoctemp.locx = merrorcheck.at( merrorcheck.size() - 1 ).locx ;
              mTypeAndLoctemp.locy = merrorcheck.at( merrorcheck.size() - 1 ).locy ;
              mTypeAndLoctemp.type = "ATOM" ;
              merrorcheck.pop_back() ;
              mdone = true ;
            } // if

            if ( mtokenstring.size() == 0 && mread == '(' && mquote ) {
              for ( int i = mquotenum ; i > 0 ; i-- ) {
                mtokenstring.push_back( "\'" ) ;
                merrorcheck.push_back( mOuterQuote.at( mquotenum - i ) ) ;
              } // for

              mquotenum = 0 ;
            } // if

            if ( mtoken != "(" && mtoken != ")" && mtoken != "\'" ) {
              if ( mtoken == "." )
                mTypeAndLoctemp.type = "DOT" ;
              else
                mTypeAndLoctemp.type = "ATOM" ;

              if ( mtoken.at( mtoken.size() - 1 ) == '\"' ) 
                mTypeAndLoctemp.locx = mx - mtoken.size() + 1 ;
              else {
                mTypeAndLoctemp.locx = mx - mtoken.size() ;
                if ( mconvert && mstringM )
                  mTypeAndLoctemp.locx = mTypeAndLoctemp.locx + 2 ;
                else if ( mTypeAndLoctemp.locx == 0 || mstringM || mconvert || mread == '\n' )
                  mTypeAndLoctemp.locx++ ;
                mconvert = false ;
              } // else

              mTypeAndLoctemp.locy = my ;
            } // if
            else if ( !mdone ) {
              if ( mtoken == "(" )
                mTypeAndLoctemp.type = "LP" ;
              else if ( mtoken == ")" )
                mTypeAndLoctemp.type = "RP" ;
              else
                mTypeAndLoctemp.type = "QUOTE" ;

              mTypeAndLoctemp.locx = mx ;
              mTypeAndLoctemp.locy = my ;              
            } // else if

            mdone = false ;
            mtokenstring.push_back( mtoken ) ;
            merrorcheck.push_back( mTypeAndLoctemp ) ;
            merrorcheck_loc = 0 ;
            mpass = Syntax() ;
            if ( !mpass ) {
              if ( mquote )
                mquote = false ;
              mleftmode = false ;
              mError = true ;
              mThisLineHasSth = true ;
            } // if

            if ( ( mtokenstring.size() == 3 && mtokenstring.at( 1 ) == "exit" &&
                   mtokenstring.at( 2 ) == ")" && mtokenstring.at( 0 ) == "(" ) ||
                 ( mtokenstring.size() == 5 && mtokenstring.at( 1 ) == "exit" &&
                   mtokenstring.at( 4 ) == ")" && mtokenstring.at( 0 ) == "(" &&
                   mtokenstring.at( 2 ) == "." && mtokenstring.at( 3 ) == "nil" 
                 ) ) { 
              mover = true ;
              mleftmode = false ;
            } // if
          } // if

          if ( mread == '\n' ) {
            mx = 0 ;
            my++ ;
          } // if

          if ( mnumofL == 0 ) { // Convert to number and check syntax
            for ( int i = 0 ; i < mtokenstring.size() ; i++ )
              mtokenstring.at( i ) = Identify2( mtokenstring.at( i ) ) ;

            mleftmode = false ;
          } // if

          // for ( int i = 0 ; i < mtokenstring.size() ; i++ )
            // cout << mtokenstring.at( i ) << " " ;
          // cout << endl ;
          // cout << mtokenstring.size() << " size " << endl ;
          // for ( int i = 0 ; i < merrorcheck.size() ; i++ )
            // cout << merrorcheck.at( i ).locx << ", " << merrorcheck.at( i ).locy 
            // << " " << merrorcheck.at( i ).type << endl ;
          // cout << endl ;
          // cout << merrorcheck.size() << " size " << endl ;

          if ( mnumofL == 0 && !mover && mpass ) { 
            Node *head = new Node ;
            NewNode( head, "" ) ;
            Node *walk = head ;
            cout << "> " ;
            BuildTree() ;
            BuildTree2() ;
            mquoteloc.push_back( 500 ) ;
            Node *non = mnotElist[ mnotElist.size() - 1 ] ;
            mtnode = mnodelist[ mnodelist.size() - 1 ] ;
            PrintS( EvalExp( mtnode, non, true ) ) ;
            if ( mover )
              mrepeat = true ;
            // PrettyPrint( mtokenstring, 0, 0, 0, 0, 0, 0 ) ;
            if ( !mover )
              cout << endl ;
            mquote = false ;
            mtokenstring.clear() ;
            merrorcheck.clear() ; 
            mx = 0 ;
            my = 1 ;
            mquotenum = 0 ; 
            mOuterQuote.clear() ;
            mquotefirst = true ;
            mThisLineHasSth = true ;
          } // if 
        } // else if
        else if ( mtoken != "" && !mError && !mleftmode && mtoken != "\'" &&
                  !msymbolerror ) { // for general token
          for ( int i = mquotenum ; i > 0 ; i-- )
            mtokenstring.push_back( "\'" ) ;
          mtokenstring.push_back( mtoken ) ;
          int spacenum = 0 ;
          if ( mread == '\n' || mread == ')' || mstringM )
            mx = 0 ;
          else {
            mx = 1 ;
          } // else 
          
          my = 1 ;
          /*
          if ( mquote ) { 
            for ( int i = mquotenum ; i > 0 ; i-- ) { 
              for ( int i = 0 ; i < spacenum ; i++ ) 
                cout << " " ;
              cout << "( quote" << endl ;
              spacenum = spacenum + 2 ;
            } // for
          } // if

          if ( mquote ) { 
            for ( int i = 0 ; i < spacenum ; i++ ) 
              cout << " " ;
            spacenum = spacenum - 2 ;
          } // if

          cout << mtoken << endl ;

          if ( mquote ) { 
            for ( int i = mquotenum ; i > 0 ; i-- ) { 
              for ( int i = spacenum ; i > 0 ; i-- ) 
                cout << " " ;
              cout << ")" << endl ;
              spacenum = spacenum - 2 ;
            } // for
          } // if
          */
          BuildANode() ;
          mtnode = mnodelist[ mnodelist.size() - 1 ] ;
          PrintS( EvalExp( mtnode, NULL, true ) ) ;
          cout << endl ;
          mThisLineHasSth = true ;
          mquote = false ;
          mtokenstring.clear() ;
          mquotenum = 0 ;
          mOuterQuote.clear() ;
        } // else if

        // cout << mread << " this is mtoken !!" << endl ;
        if ( mread == '\n' && !mleftmode ) { // Finish a token
          if ( !mcomment && !mThisLineHasSth ) {
            my++ ; 
          } // if

          if ( ( mError || !mcomment ) && mThisLineHasSth )
            my = 1 ;
          mThisLineHasSth = false ;
          mdoterror = false ;
          mError = false ;
          mnoclosingquote = false ;
          mfirst = true ;
          mcomment = false ;
          mtokenstring.clear() ; 
          merrorcheck.clear() ; 
          mnumofL = 0 ;
          mdot = 1 ;
          mx = 0 ;
        }  // if

        mnum = true ;     
        mtoken = "" ;
        mstringM = false ;
        mresult = 1 ;
        mfloat = false ;
        msymbolerror = false ;
        mstringappend = "" ;
        mstage = 1 ;
      } // if

      // for ( int i = 0 ; i < mtokenstring.size() ; i++ )
        // cout << mtokenstring.at( i ) << " " ;
      // cout << endl ;
      // cout << mtokenstring.size() << " size " << endl ;
      if ( mover && !mrepeat )
        cout << "> " ; 
    } // while

    if ( !mover ) {
      cout << "> ERROR (no more input) : END-OF-FILE encountered" ; 
    } // if
  } // ReadAndToken()

} ;

int main() { 
  int uTestNum ;
  cout << "Welcome to OurScheme!" << endl << endl ;
  cin >> uTestNum ;
  OurScheme a ;
  a.BuildFunctionList() ;
  a.ReadAndToken() ;
  cout << "\nThanks for using OurScheme!" ;
} // main()


