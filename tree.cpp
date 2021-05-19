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
}



int dfs1(Arbre& a, int x, int s, int pre) {
    int i;
    for (i = 0; i <= N; i++) {
        if (i != pre && a.g[x][i]) {
            if (a.dp[i] == -1) {
                if (a.dp[x] > a.g[x][i]) {
                    a.dp1[i].clear();
                    for (auto itE : a.dp1[x]) {
                        a.dp1[i].push_back(itE);
                    }
                    a.dp[i] = a.dp[x];
                    //if (s == i) max = a.dp[x];
                }
                else if (a.dp[x] == a.g[x][i]) {
                    Edge* pe = new Edge;
                    pe->poids = a.g[x][i];
                    pe->src = x < i ? x : i;   //记录这条边
                    pe->dest = x > i ? x : i;
                    a.dp1[x].push_back(*pe);
                    delete pe;
                }
                else {  //a.dp1[x].poids < a.g[x][i]
                    a.dp1[i].clear();
                    Edge* pe = new Edge;
                    pe->poids = a.g[x][i];
                    pe->src = x < i ? x : i;   //记录这条边
                    pe->dest = x > i ? x : i;
                    a.dp1[i].push_back(*pe);
                    //if (s == i) max = a.g[x][i];
                    a.dp[i] = a.g[x][i];
                    delete pe;
                }
            }
            dfs1(a, i, s, x);
        }
    }
    return a.dp[s];
}

// trouver un arête qui est de poids maximum et relié avec point x parmis les arêtes pour allant de x au point s
int dfs2(Arbre& a, int x, int xc, int pre, int s){
    int flag = 0;

    for (int i = 0; i <= N; i++){
        if (i != pre && a.g[x][i]){
            if(i==s){
                flag = 1;
            }else{
                dfs2(a, i, x, s);
            }
            if(x==xc && flag){
                a.dp2.src = x;
                a.dp2.dest = i;
                a.dp2.poids = a.g[x][i];
                return a.dp2.poids;
            }
        }
    }
    return -1;
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

// optimiser l'arbre couvrant pour trouver les autres arbres couvrants de poids minimum
void optimizer(Graph& G){
    for(auto iterE:G.nonEdges){
        for(auto iterA:G.Tmini){
            int max = 0;    //poids d'arête à supprimer
            //还没初始化dp！！！
        
            //tous les deux sommets sont full
            if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]==G.limite[iterE.dest]){
                continue;
            }

            //tous les deux sommets ne sont pas full
            if(iterA.m[iterE.src]<G.limite[iterE.src] && iterA.m[iterE.dest]<G.limite[iterE.dest]){
                max = dfs1(iterA,iterE.src,-1); //!!!
                if (iterE.poids < max) continue;
                if (iterE.poids == max) {
                    for (auto itE : G.dp1[iterE.dest]) {
                        Arbre* pa = new Arbre(iterA);
                        minusEdge(*pa, itE);
                        plusEdge(*pa, iterE);
                        G.Tmini.push_back(*pa);
                        delete pa;
                    }
                }else{   //iterE.poids > max
                    G.Tmini.clear();
                    for (auto itE : G.dp1[iterE.dest]) {
                        Arbre* pa = new Arbre(iterA);
                        minusEdge(*pa, itE);
                        plusEdge(*pa, iterE);
                        G.Tmini.push_back(*pa);
                        delete pa;
                    }
                }
                continue;
            }
        
            G.dp2.clear();
            //src est full et dest n'est pas full
            if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]<G.limite[iterE.dest]){
                max = dfs2(iterA,iterE.src, iterE.src, -1,iterE.dest); //!!!
            }
            //src est full et dest n'est pas full
            if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]==G.limite[iterE.dest]){
                max = dfs2(iterA,iterE.dest, iterE.dest, -1,iterE.src); //!!!
            }

            if (iterE.poids > max) continue;
            if (iterE.poids == max) {
                Arbre* pa = new Arbre(iterA);
                minusEdge(*pa, G.dp2);
                plusEdge(*pa, iterE);
                G.Tmini.push_back(*pa);
                delete pa;
                continue;
            }
            else{    //iterE.poids < max
                G.Tmini.clear();
                G.mini = G.mini + iterE.poids - max;
                Arbre* pa = new Arbre(iterA);
                minusEdge(*pa, G.dp2);
                plusEdge(*pa, iterE);
                G.Tmini.push_back(*pa);
                delete pa;
            }
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
