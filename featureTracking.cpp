// Homework.cpp : Defines the entry point for the console application.

// NAME: NAREN NAGARAJAN
// ECE 847 ASSIGNMENT 6

#include <afxwin.h>// necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"
#include<math.h>
#include<stack>
#include<vector>
#include<queue>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace blepo;
using namespace std;
int max_disp;
ImgFloat gradient(ImgGray image,double mu,double g[],double gd[])
{
	ImgFloat tempo;
	ImgFloat tempi;
	ImgFloat in;
	tempi.Reset(image.Width(),image.Height());
	tempo.Reset(image.Width(),image.Height());
	Set(&tempi,0);
	Set(&tempo,0);
	Convert(image,&in);
	int w=2*(int)mu+1;
	float val;
	for(int y=0;y<image.Height();y++)		
	{
		for(int x=mu;x<image.Width()-mu;x++)
		{
			val=0;
			for(int i=0;i<w;i++)
				val=val+(gd[i]*in(x+mu-i,y));
			tempi(x,y)=val;
		}
	}
	for(int y=mu;y<image.Height()-mu;y++)
	{
		for(int x=0;x<image.Width();x++)
		{
			val=0;
			for(int i=0;i<w;i++)
				val=val+(g[i]*tempi(x,y+mu-i));
			tempo(x,y)=val;
		}
	}
	return tempo;
}
void compmatrix1(ImgFloat gx,ImgFloat gy, int i, int j, float &xx,float &xy,float &yy)
{
	xx=yy=xy=0;
	for(int y=-1;y<2;y++)
	{
		for(int x=-1;x<2;x++)
		{
			xx=xx+(gx(i+x,j+y)*gx(i+x,j+y));
			yy=yy+(gy(i+x,j+y)*gy(i+x,j+y));
			xy=xy+(gx(i+x,j+y)*gy(i+x,j+y));
		}
	}
}
ImgFloat nonmaximal(ImgFloat image)
{
	ImgFloat gradcheck;
	gradcheck.Reset(image.Width(),image.Height());
	Set(&gradcheck,0);
	for(int j=1;j<image.Height()-1;j++)
		for(int i=1;i<image.Width()-1;i++)
		{
			if(image(i-1,j-1)>image(i,j)||image(i+1,j+1)>image(i,j))
				gradcheck(i,j)=0;
			else if(image(i-1,j+1)>image(i,j)||image(i+1,j-1)>image(i,j))
				gradcheck(i,j)=0;
			else if(image(i,j-1)>image(i,j)||image(i,j+1)>image(i,j))
				gradcheck(i,j)=0;
			else if(image(i-1,j)>image(i,j)||image(i+1,j)>image(i,j))
				gradcheck(i,j)=0;
			else gradcheck(i,j)=image(i,j);
		}
	return gradcheck;
}
float bilinear(ImgGray image, float x, float y)
{
	int x0=floor(x);
	int y0=floor(y);
	float ax= x-x0;
	float ay=y-y0;

	if (x0<0)x0=0;
	if(y0<0)y0=0;
	if(x0>=image.Width()-1) x0=image.Width()-2;
	if(y0>=image.Height()-1) y0=image.Height()-2;

	float a=((1-ax)*(1-ay)*image(x0,y0))+(ax*(1-ay)*image(x0+1,y0))+((1-ax)*ay*image(x0,y0+1))+(ax*ay*image(x0+1,y0+1));
	
	return a;
}
float bilinear(ImgFloat image, float x, float y)
{

	int x0=floor(x);
	int y0=floor(y);
	float ax= x-x0;
	float ay=y-y0;

	if (x0<0)x0=0;
	if(y0<0)y0=0;
	if(x0>=image.Width()-1) x0=image.Width()-2;
	if(y0>=image.Height()-1) y0=image.Height()-2;

	float a=((1-ax)*(1-ay)*image(x0,y0))+(ax*(1-ay)*image(x0+1,y0))+((1-ax)*ay*image(x0,y0+1))+(ax*ay*image(x0+1,y0+1));
	return a;
}
void computematrixlkt(ImgFloat gx,ImgFloat gy, float i, float j, float &xx,float &xy,float &yy,int window)
{
	xx=yy=xy=0;
	int win=window/2;
	for(int y=-(window-1)/2;y<(window+1)/2;y++)
	{
		for(int x=-(window-1)/2;x<(window+1)/2;x++)
		{
			xx=xx+bilinear(gx,i+x,j+y)*bilinear(gx,i+x,j+y);
			yy=yy+bilinear(gy,i+x,j+y)*bilinear(gy,i+x,j+y);
			xy=xy+bilinear(gx,i+x,j+y)*bilinear(gy,i+x,j+y);
		}
	}
}
void errorvector(ImgFloat gx,ImgFloat gy,ImgGray image1,ImgGray image2,int window, float u[], float i, float j, float &ex, float &ey)
{
	ex=0;
	ey=0;
	int win=window/2;
	for(int y=-(window-1)/2;y<(window+1)/2;y++)
		for(int x=-(window-1)/2;x<(window+1)/2;x++)
		{
			ex=ex+(bilinear(gx,x+i,y+j)*(bilinear(image1,x+i,y+j)-bilinear(image2,x+i+u[0],y+j+u[1])));
			ey=ey+(bilinear(gy,x+i,y+j)*(bilinear(image1,x+i,y+j)-bilinear(image2,x+i+u[0],y+j+u[1])));
		}
}
void linearsytem(float xx, float yy, float xy, float ex, float ey, float u1[])
{
	float det=(xx*yy)-(xy*xy);
	if(det!=0)
	{
		u1[0]=(1/det)*((yy*ex)-(xy*ey));
		u1[1]=(1/det)*((xx*ey)-(xy*ex));
	}
}
int main(int argc, const char* argv[], const char* envp[])
{
	// Initialize MFC and return if failure
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule == NULL || !AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		printf("Fatal Error: MFC initialization failed (hModule = %x)\n", hModule);
		return 1;
	}
	try
	{
		if(argc==6)
		{
			ImgGray image;
			ImgBgr bgrimage;
			CString filename;
			int framestart=atoi(argv[2]);
			int frameend=atoi(argv[3]);
			string format=argv[1];
			string inputfile;
			string ext="../../images/";
			int i=framestart;
			filename.Format(format.c_str(),i);
			inputfile=ext.c_str()+filename;
			Load(inputfile.c_str(),&image);
			Load(inputfile.c_str(),&bgrimage);
			ImgBgr finalimage;
			i=frameend;
			filename.Format(format.c_str(),i);
			inputfile=ext.c_str()+filename;
			Load(inputfile.c_str(),&finalimage);
//Kernels
			double sigma=1;
			double mu=2.5*sigma-0.5;
			int w=2*(int)mu+1;
			double* g=(double*)calloc(w,sizeof(double));
			double* gd=(double*)calloc(w,sizeof(double));
			double sumg=0,sumgd=0;
			for(i=0;i<w;i++)
			{
				g[i]=exp(-(i-mu)*(i-mu)/(2*sigma*sigma));
				gd[i]=(i-mu)*g[i];
				sumg=sumg+g[i];
				sumgd=sumgd+(gd[i]*i);
			}
			for(i=0;i<w;i++)
			{
				g[i]=g[i]/sumg;//1d gaussian kernel	
				gd[i]=-gd[i]/sumgd;//1d gaussian derivative kernel
			}
//Gradient Images
			ImgFloat gradx=gradient(image,mu,g,gd);
			ImgFloat grady=gradient(image,mu,gd,g);
			ImgFloat gradxx,gradyy,gradxy;
//Tomasi Kanade
			vector<float> z;
			float lambda1,lambda2;
			float xx,xy,yy;
			ImgFloat cornerness(image.Width(),image.Height());
			Set(&cornerness,0);
			int threshold=500;
			for(int j=1;j<image.Height()-1;j++)
				for(int i=1;i<image.Width()-1;i++)
				{
					compmatrix1(gradx,grady,i,j,xx,xy,yy);
					lambda1=0.5*(xx+yy+sqrt(((xx-yy)*(xx-yy))+(4*xy*xy)));
					lambda2=0.5*(xx+yy-sqrt(((xx-yy)*(xx-yy))+(4*xy*xy)));
					if(lambda2>threshold)
					{
						cornerness(i,j)=lambda2;
					}
					else 
						cornerness(i,j)=0;
				}
			Figure features("Cornerness Image");
			features.Draw(cornerness);
			ImgFloat gradcheck=nonmaximal(cornerness);
			Figure gradientcheck("Non Maximal Suppression ");
			gradientcheck.Draw(gradcheck);
			vector<Point> featurepts;
			Point p;
			int count=0;
			Bgr colour;
			for(int j=1;j<image.Height()-1;j++)
				for(int i=1;i<image.Width()-1;i++)
					if(gradcheck(i,j)>threshold)
					{
						count++;
						p.x=i;
						p.y=j;
						featurepts.push_back(p);
						DrawCircle(p,1,&bgrimage,colour.RED,1);
					}
			Figure colorimage("Features using Tomasi Kanade ");
			colorimage.Draw(bgrimage);
			cout<<"\nFeature points = \n";

//Lucas Kanade			
			ImgBgr finaloutput;
			Figure finalop("Final output with LKT");
			int window=atoi(argv[5]);
			float u[2];
			u[0]=u[1]=0;
			float sigma1=atof(argv[4]);
			float x,y;
			int iter=0;
			int maxiter=2;
			int th=5;
			double mu1=2.5*sigma1-0.5;
			int w1=2*(int)mu1+1;
			double* g1=(double*)calloc(w1,sizeof(double));
			double* gd1=(double*)calloc(w1,sizeof(double));
			double sumg1=0,sumgd1=0;

			for(int i=0;i<w1;i++)
			{
				g1[i]=exp(-(i-mu1)*(i-mu1)/(2*sigma1*sigma1));
				gd1[i]=(i-mu1)*g1[i];
				sumg1=sumg1+g1[i];
				sumgd1=sumgd1+(gd1[i]*i);
			}
			for(int i=0;i<w1;i++)
			{
				g1[i]=g1[i]/sumg1;//1d gaussian kernel	
				gd1[i]=-gd1[i]/sumgd1;//1d gaussian derivative kernel
			}
			ImgGray image1;
			ImgGray I,J;
			ImgFloat gradx1;
			ImgFloat grady1;
			gradx1.Reset(image.Width(),image.Height());
			grady1.Reset(image.Width(),image.Height());
			
//loop		
			float ex,ey;
			float u1[2]={0};	
			float xx1,xy1,yy1;
			for(int i=framestart;i<frameend;i++)
			{
				filename.Format(format.c_str(),i);
				inputfile=ext.c_str()+filename;
				Load(inputfile.c_str(),&image1);
				Load(inputfile.c_str(),&I);
				filename.Format(format.c_str(),i+1);
				inputfile=ext.c_str()+filename;
				Load(inputfile.c_str(),&J);
			
//Gradient Images for lkt

				gradx1=gradient(I,mu1,g1,gd1);
				grady1=gradient(I,mu1,gd1,g1);
				for(int j=0;j<count;j++)
				{
					u[0]=u[1]=0;
					iter=0;
					computematrixlkt(gradx1,grady1,featurepts[j].x,featurepts[j].y,xx1,xy1,yy1,window);
					while(iter<3)
					{
						errorvector(gradx,grady,I,J,window,u,featurepts[j].x,featurepts[j].y,ex,ey);
						linearsytem(xx1,yy1,xy1,ex,ey,u1);
						u[0]=u[0]+u1[0];
						u[1]=u[1]+u1[1];
						iter++;
					}
					featurepts[j].x=featurepts[j].x+u[0];
					featurepts[j].y=featurepts[j].y+u[1];
				}
				Convert(J,&finaloutput);
				for(int k=0;k<count;k++)
				{
					DrawCircle(featurepts[k],1,&finaloutput,colour.RED,1);
				}
			finalop.Draw(finaloutput);
			}
		}
		else cout<<"\n Number of Arguments not equal to 5";
		EventLoop();
	}
	catch(const Exception &p)
	{
		p.Display();
	}
	return 0;
}
