#include<cstdio>
#include<iostream>
#include<vector>
#include<cstring>

using namespace std;

#define N 5     //Le nombre de sommets dans le graphe d'origine
#define INF 100

int size = N*sizeof(int);

// classe de arete
class Edge {
public:
    int src, dest, poids; //source,destination et poids pour chaque arete

    // classer les arêtes par les poids de ordre décroissant 
    friend bool operator < (Edge e1, Edge e2){
        return e1.poids < e2.poids;
    }
  Edge& operator = (Edge& e2){
    this->src = e2.src;
    this->dest = e2.dest;
    this->poids = e2.poids;
    return *this;
  }
  Edge(){};
  Edge(const Edge& e);
};

Edge::Edge(const Edge& e):src(e.src),dest(e.dest),poids(e.poids){}

class Arbre {
public:
    int sommets[N];
    int g[N][N];
    vector <class Edge> edges;
    map<int,class edges> mapE;
    int m[N];
    Edge dp1[N];    //点到root的路径上的最大边
    Edge dp2;    //点到root的路径上与这个点相连的边
    int poids;
    Arbre();
    Arbre(const Arbre& a);
};

Arbre::Arbre() {
    
    memset(this->sommets, 0, N*sizeof(int));
    memset(this->g, 0, N * N*sizeof(int));
    memset(this->m, 0, N * sizeof(int));
    this->poids = 0;

}

Arbre::Arbre(const Arbre& a){
  for(int i=0;i<N;i++){
    this->sommets[i] = a.sommets[i];
    this->m[i] = a.m[i];
    for(int j=0;j<N;j++){
      this->g[i][j] = a.g[i][j];
    }
  }
  this->poids = a.poids;
}

// classe pour le graphique (arbre)
class Graph {
public:
  int V,E;    //nombre de sommets et d'aretes
  int sommets[N]; //liste de sommets
  vector <class Edge> edges;  // liste d'aretes
  int g[N][N];  // liste d'adjacent (valeur)
  int father[N]; // parent pour les sommets 
  int limite[N]; //limite de degre pour chaque sommet
  int mini;
  vector<class Graph> sousG; //les sous-ensembles de graphe
  vector<Edge> nonEdges;
  vector<class Arbre> Tmini; //les arbres couvrants
  Graph();
  Graph(int V,int E,int lim[N],int g[N][N]);
};

Graph::Graph(){
  
  //edges = new list<class Edge>[E];  // créer un liste d'arete de taille E
    this->mini = 0;
     // initialisation
    for(int i=0;i<N;i++){
      sommets[i] = 0;
      for(int j=0;j<N;j++)
	g[i][j] = INF;
    }
    //edeges.sort(); //Trier les aretes du plus petit au plus grand
}

Graph::Graph(int V,int E,int lim[N],int g[N][N]):V(V),E(E)
{
  
  //memcpy(sommets,s,size);
  //edges = new list<class Edge>[E];  // créer un liste d'arete de taille E
    this->mini = 0;
    // initialisation
    
    for(int i=0;i<V;i++){
        this->father[i] = i;
        this->sommets[i]=1;
	this->limite[i] = lim[i];
	for(int j=0;j<N;j++){
	  this->g[i][j] = g[i][j];
	}
    }
    
    //sort(this->edges.begin(),this->edges.end()); //Trier les aretes du plus petit au plus grand
}

int creatEdgesG(Graph& G) {
    int nbE=0;
    Edge* p;
    for (int i = 0; i < N; i++)
      for (int j = i + 1; j < N; j++){
	if (G.g[i][j] != INF) {
	  nbE += 1;
	  p = new Edge;
	  p->src = i;
	  p->dest = j;
	  p->poids = G.g[i][j];
	  //cout<<i<<","<<j<<endl;
	  G.edges.push_back(*p);
	  delete p;
	}
      }
    return nbE;
  }

void plusEdge(Arbre& a, Edge e){
  a.g[e.src][e.dest]=e.poids;
  a.g[e.dest][e.src]=e.poids;
  a.poids+=e.poids;
  a.m[e.src]++;
  a.m[e.dest]++;
  a.sommets[e.src] = a.sommets[e.dest] = 1;
  /*
  int identique_src = 0;
  int identique_dest = 0;
  for(int i=0; i<N; i++){
    if(sommets[i]==e.src){
      identique_src = 1;
    }
    else if(sommets[i]==e.dest){
      identique_dest = 1;
    }
  }
  if(identique_src == 0){ sommets=e.src;}
  if(identique_dest == 0){ sommet+=e.dest;}
  */
}

void dfs1(Arbre& a, int x, int pre){  //dfs求root到某节点路程上的最大值
  int i;
  for (i = 0; i <= N; i++){
    if (i != pre && a.g[x][i]){
      if (a.dp1[i].poids == -1){
	if (a.dp1[x].poids > a.g[x][i]){
	  a.dp1[i] = a.dp1[x];
	}else{
	  a.dp1[i].poids = a.g[x][i];
	  a.dp1[i].src = x;   //记录这条边
	  a.dp1[i].dest = i;
	}
      }
      dfs1(a, i, x);
    }
  }
}


void dfs2(Arbre& a, int x, int pre, int s){
  //dfs求x到s路程上的与x相连的边
  int flag = 0;
  for (int i = 0; i <= N; i++){
    if (i != pre && a.g[x][i]){
      if(i==s){
	      flag = 1;
      }else
	dfs2(a, i, x, s);
    }
    if(flag){
      a.dp2.src = x;
      a.dp2.dest = i;
      a.dp2.poids = a.g[x][i];
      break;
    }
  }
}



void trouver(Graph& G){
  Arbre* pa = new Arbre;
  //int s = 0;
  G.nonEdges.clear();
  for(auto iterE:G.edges){
    //if(s==N-1) break;
    if(pa->sommets[iterE.src] && pa->sommets[iterE.dest]) {
      G.nonEdges.push_back(iterE);
      continue;
    }
    
    if(pa->m[iterE.src]==G.limite[iterE.src] || pa->m[iterE.dest]==G.limite[iterE.dest]) {
      G.nonEdges.push_back(iterE);
      continue;
    }

    plusEdge(*pa,iterE);
    cout<<iterE.src<<","<<iterE.dest<<endl;
  }
  G.Tmini.push_back(*pa);
  delete pa;
}


void optimizer(Graph& G){
  for(auto iterE:G.nonEdges){
    for(auto iterA:G.Tmini){
      //tous les deux sommets sont full
      if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]==G.limite[iterE.dest]){
        continue;
      }
      //tous les deux sommets ne sont pas full
      if(iterA.m[iterE.src]<G.limite[iterE.src] && iterA.m[iterE.dest]<G.limite[iterE.dest]){
        dfs1(iterA,iterE.src,-1); //!!!
        if(iterE.poids < G.dp1[iterE.src])
      }
      
      continue;
      //src est full et dest n'est pas full
      if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]<G.limite[iterE.dest]){
        dfs2(iterA,iterE.src,-1,iterE.dest); //!!!
      }continue;
      //src est full et dest n'est pas full
      if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]==G.limite[iterE.dest]){
        dfs2(iterA,iterE.dest,-1,iterE.src); //!!!
      }continue;
    }
  }
}


int main(){
  //int s[5] = {1,1,1,1,1};
  int lim[N] = {1,3,1,2,2};
  int a[N][N] = {{INF,1,INF,INF,1},
		  {1,INF,1,1,1},
		  {INF,1,INF,1,INF},
		  {INF,1,1,INF,1},
		  {1,1,INF,1,INF}};
  Graph G1(5,7,lim,a);
  //memcpy(G1.g,a,sizeof(a));
  //memcpy(G1.limite,lim,sizeof(lim));
  creatEdgesG(G1);
  //regrouper(G1);
  trouver(G1);

  for(int i=0;i<N;i++){
    cout<<i<<":"<<endl;
    for(int j=0;j<N;j++){
      if(G1.Tmini[0].g[i][j])
	    cout<<j<<endl;
    }
  }
  
  return 1;
}
