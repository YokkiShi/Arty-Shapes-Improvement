// class Affinity_multiscale_hist
// code based on I. Omer and M. Wermann. The Bottleneck Geodesic: Computing Pixel Affinity. CVPR, 2006.
// Timothee Cour, 04-Aug-2008 20:46:38 -- DO NOT DISTRIBUTE




#include <math.h>
#include <matrix.h>
#include <mex.h>
#include <vector>

using namespace std;

class Affinity_multiscale_hist : public Affinity {
public: 
	Affinity_multiscale_hist(int*ir2_, int*jc2_,int n_,const mxArray*mxArray_options_);

private:
	void readOption();
	double computeWij(int i,int j);

	//int p0,q0,n0;
	int*location;
	int nI;

	int pH,qH,kH;
	int pI,qI;

	int kF;
	double*valF;
	//double*hist;
	double*neg_hist;
	int*map;

	double*ephase;
	bool isPhase;
};


Affinity_multiscale_hist::Affinity_multiscale_hist(int*ir2_, int*jc2_, int n_,const mxArray*mxArray_options_) : Affinity(ir2_,jc2_,n_,mxArray_options_) {
	readOption();
}

void Affinity_multiscale_hist::readOption(){
	const mxArray *temp;
	temp = mxGetField(mxArray_options, 0,"F");
	valF=mxGetPr(temp);
	kF=getSize3(temp);

	temp = mxGetField(mxArray_options, 0,"hist");
	//hist=mxGetPr(temp);
	neg_hist=mxGetPr(temp);
	getSizes(temp,pH,qH,kH);

	temp = mxGetField(mxArray_options, 0,"map");
	pI=mxGetM(temp);
	qI=mxGetN(temp);
	map=readInt32(temp);
	scalar_plus_vec_self(-1,map,pI*qI);

	temp = mxGetField(mxArray_options, 0,"ephase");
	ephase=mxGetPr(temp);

	temp = mxGetField(mxArray_options, 0,"isPhase");
	isPhase=(bool)mxGetScalar(temp);

	temp = mxGetField(mxArray_options, 0,"location");
	//int*location0=(int*)mxGetData(temp);
	int*location0=readInt32(temp);
	location=createArray(n,(int)0);
	for(int i=0;i<n;i++)
		location[i]=location0[i]-1;

	nI=pI*qI;

}
double Affinity_multiscale_hist::computeWij(int i,int j){
	double a=2;

	double w=0;
	int xi, yi, xj, yj;

	if (i==j){
		w = 1;
		return w;
	}

	int zi=location[i];
	int zj=location[j];

	//xi = zi / p0;
	//yi = zi % p0;
	//xj = zj / p0;
	//yj = zj % p0;

	double dist=euclidian_dist(zi,zj,valF,nI,kF);
	//double dist=euclidian_dist(i,j,valF,n,kF);

	int indHisti=map[i];
	int indHistj=map[j];
	xi = indHisti / pH;
	yi = indHisti % pH;
	xj = indHistj / pH;
	yj = indHistj % pH;

	//double maxi = max_over_segment(xi,yi,xj,yj,hist,ephase,pH,qH,kH,isPhase);
	double mini = -max_over_segment(xi,yi,xj,yj,neg_hist,ephase,pH,qH,kH,isPhase);

	//double temp=min(hist[indHisti],hist[indHistj]);
	double temp=min(-neg_hist[indHisti],-neg_hist[indHistj]);

	double bottleneck=a*temp/(a*mini+temp);

	w = exp(-dist*bottleneck);
	return w;
}
