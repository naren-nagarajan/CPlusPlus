// Homework.cpp : Defines the entry point for the console application.
//


// NAME: NAREN NAGARAJAN
// ECE 847 ASSIGNMENT 2


#include <afxwin.h>// necessary for MFC to work properly
#include<string.h>//for strlen()
#include "Homework.h"
#include "../../src/blepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace blepo;

double mcount[20];
double area[20]={0};
int areacount=0;
double compactness[20];
int compactcount=0;
double centroidx[20]={0};
double centroidy[20]={0};
double m00[20]={0};
double m01[20]={0};
double m10[20]={0};
double m20[20]={0};
double m02[20]={0};
double m11[20]={0};
double mu00[20]={0};
double mu11[20]={0};
double mu20[20]={0};
double mu02[20]={0};
double e[20]={0};
double direction[20]={0};

void moment(ImgGray l)
{
	mcount[0]=mcount[1]=mcount[2]=mcount[3]=mcount[4]=mcount[5]=0;
	for(int j=0;j<l.Height();j++)
		for(int i=0;i<l.Width();i++)
			if(l(i,j)==255)
			{
				mcount[0]+=l(i,j);
				mcount[1]+=l(i,j)*i;
				mcount[2]+=l(i,j)*j;
				mcount[3]+=l(i,j)*i*i;
				mcount[4]+=l(i,j)*j*j;
				mcount[5]+=l(i,j)*i*j;
			}
	mcount[6]=mcount[0];
	mcount[10]=mcount[1]/mcount[0];
	mcount[11]=mcount[2]/mcount[0];
	mcount[7]=(mcount[5]-mcount[11])*mcount[1];
	mcount[8]=(mcount[3]-mcount[10])*mcount[1];
	mcount[9]=(mcount[4]-mcount[11])*mcount[2];
	double x=mcount[8]-mcount[9];
	mcount[12]=mcount[8]+mcount[9]-sqrt((x*x)+(4*mcount[7]*mcount[7]));
	mcount[13]=mcount[8]+mcount[9]+sqrt((x*x)+(4*mcount[7]*mcount[7]));
	mcount[14]=sqrt((mcount[13]-mcount[12])/mcount[13]);
	mcount[15]=atan2((2*mcount[7]),(mcount[8]-mcount[9]));
	area[areacount]=mcount[6];
	centroidx[areacount]=mcount[10];
	centroidy[areacount]=mcount[11];
	m00[areacount]+=mcount[0];
	m10[areacount]+=mcount[1];
	m01[areacount]+=mcount[2];
	m20[areacount]+=mcount[3];
	m02[areacount]+=mcount[4];
	m11[areacount]+=mcount[5];
	mu00[areacount]+=mcount[6];
	mu11[areacount]+=mcount[7];
	mu20[areacount]+=mcount[8];
	mu02[areacount]+=mcount[9];
	e[areacount]+=mcount[14];
	direction[areacount]+=mcount[15];
	//cout<<area[areacount]<<endl;
	areacount++;
	//cout<<mcount[0]<<endl<<mcount[1]<<endl<<mcount[2]<<endl<<mcount[3]<<endl<<mcount[4]<<endl<<mcount[5];
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
		if (argc==2)
		{

//a part: Loading Bgr Image and Displaying it 
			ImgGray lowthresh;
			ImgGray highthresh;
			ImgBgr output;
			string filename; 
			string ext="../../images/";
			filename=ext+argv[1];
			Load(filename.c_str(), &lowthresh);
			Load(filename.c_str(), &highthresh);
			Load(filename.c_str(), &output);
			Figure fig;
			fig.Draw(output);
			fig.SetTitle(" Original Image ");

			
//Lower Thresholding
			int x,y;
			int t1=90;
			int t2=210;
			for(x=0;x<lowthresh.Width();x++)
				for(y=0;y<lowthresh.Height();y++)
				{
					if(lowthresh(x,y)<t1)// Lower limit to 0
						lowthresh(x,y)=0;
					else lowthresh(x,y)=255;
				}
			Erode3x3(lowthresh,&lowthresh);
			Dilate3x3(lowthresh,&lowthresh);
//High thresholding
			for(x=0;x<highthresh.Width();x++)
				for(y=0;y<highthresh.Height();y++)
				{
					if(highthresh(x,y)>t2)// Lower limit to 0
						highthresh(x,y)=255;
					else highthresh(x,y)=0;
				}
			Figure fig1;
			fig1.Draw(lowthresh);
			fig1.SetTitle(" Low Threshold "); // Low Threshold Image
			Figure high;
			high.Draw(highthresh);	//High Threshold Image
			high.SetTitle(" High Threshold");
// Floodfill	
			ImgBgr lowBgr;
			Convert(lowthresh,&lowBgr);
			ImgBgr floodfill;
			int i[10]={20,40,60,80,100,120,140,160,180,200};
			int j=0;
			for(y=0;y<highthresh.Height();y++)
				for(x=0;x<highthresh.Width();x++)
					if(highthresh(x,y)==255)
					{
						FloodFill8(lowBgr,x,y, Bgr(255,255,255), &floodfill); 
					}
			Erode3x3(lowthresh,&lowthresh);
			Dilate3x3(lowthresh,&lowthresh);
			for(y=0;y<floodfill.Height();y++)
				for(x=0;x<floodfill.Width();x++)
					if(floodfill(x,y).b!=255&&floodfill(x,y).g!=255&&floodfill(x,y).r!=255)
						floodfill(x,y).r=floodfill(x,y).b=floodfill(x,y).g=0;
			Figure color;
			color.Draw(floodfill);  //Double Thresholding Image
			color.SetTitle("Double Thresholding");
						
//coloured objects							
			int flag=0;
			ImgInt label;
			ImgInt* labelpointer=&label;
			std::vector< ConnectedComponentProperties<ImgGray::Pixel> > reg;  		// pixel type must match input image type
			ConnectedComponents8(floodfill,&label);
			
//Count of objects
			int a[256]={0};
			int n;
			int s=0;
			for(y=0;y<label.Height();y++)
				for(x=0;x<label.Width();x++)
				{
					n=label(x,y);
					a[n]=1;
				}
			for(int i=0;i<256;i++)
				if(a[i]==1)
					s++;
			s=s-1;
			cout<<" Number of objects = "<<s<<endl;
			int* count=(int*)calloc(s,sizeof(int));
			
			for(int j=0,i=1;i<256;i++)
				if(a[i]==1)
				{
					count[j]=i;
					j++;
				}

// Moments
			ImgGray single;
			ImgInt temp=label;
			ImgGray singleerode;
			ImgGray singleperi;
			ImgBgr stem;
			ImgGray stemerode;
			ImgGray singleorig;
			Convert(label,&single);
			Convert(label,&singleerode);
			Convert(label,&singleperi);
			Convert(label,&singleperi);
			
			Convert(label,&stemerode);
			for(int d=0;d<s;d++)
			{
				for(y=0;y<label.Height();y++)
					for(x=0;x<label.Width();x++)
					{
						if(label(x,y)!=count[d])
							single(x,y)=0;
						else single(x,y)=255;
					}
				moment(single);
				singleerode=single;
				singleorig=single;
				Erode3x3(single,&singleerode);
				int xx,yy;
				int s=0;
				for(yy=0;yy<single.Height();yy++)
					for(xx=0;xx<single.Width();xx++)
					{
						if(single(xx,yy)==singleerode(xx,yy))
							singleperi(xx,yy)=0;
						else 
						{	
							singleperi(xx,yy)=255;
							if(area[areacount-1]>=(1.25*1000000)&&area[areacount-1]<=(1.4*1000000))
							{
								output(xx,yy).b=0;
								output(xx,yy).g=255;
								output(xx,yy).r=255;
							}
							
							if(area[areacount-1]>(1.5*1000000)&&area[areacount-1]<(1.6*1000000))
							{
								output(xx,yy).b=0;
								output(xx,yy).g=128;
								output(xx,yy).r=0;
							}
							if(area[areacount-1]<=1000000&&area[areacount-1]>=700000)
							{
								output(xx,yy).b=0;
								output(xx,yy).g=0;
								output(xx,yy).r=255;
							}
							s++;
						}
					}
				//	cout<<endl<<s<<endl;
				compactness[compactcount]=((4*3.14*a[0])/(s*s));
				compactcount++;
				//cout<<endl<<endl<<compactness<<endl<<endl;
// axis
				int m,n;
				n=(int)centroidy[areacount-1];
				for(m=(int)centroidx[areacount-1]-20;m<(int)centroidx[areacount-1]+20;m++)
						output(m,n).b=output(m,n).g=output(m,n).r=0;
				m=(int)centroidx[areacount-1];
				for(n=(int)centroidy[areacount-1]-20;n<(int)centroidy[areacount-1]+20;n++)
						output(m,n).b=output(m,n).g=output(m,n).r=0;
//stem
			if(area[areacount-1]>=(1.25*1000000)&&area[areacount-1]<=(1.4*1000000))
			{
				Erode3x3(single,&single);
				Erode3x3(single,&single);
				Erode3x3(single,&single);
				Erode3x3(single,&single);
				Erode3x3(single,&single);
				Dilate3x3(single,&single);
				Dilate3x3(single,&single);
				Dilate3x3(single,&single);
				Dilate3x3(single,&single);
				Dilate3x3(single,&single);
				Erode3x3(single,&stemerode);
				for(yy=0;yy<single.Height();yy++)
					for(xx=0;xx<single.Width();xx++)
					{
						if(single(xx,yy)==singleorig(xx,yy))
							stemerode(xx,yy)=0;
						else
							stemerode(xx,yy)=255;
					}
				


				single=stemerode;
				Erode3x3(stemerode,&stemerode);
				for(yy=0;yy<single.Height();yy++)
					for(xx=0;xx<single.Width();xx++)
						if(single(xx,yy)!=stemerode(xx,yy))
						{
							output(xx,yy).b=255;
							output(xx,yy).g=0;
							output(xx,yy).r=255;
						}
				}					
			}
			int banana=0,grapefruit=0,apple=0;
			for(int i=0;i<s;i++)
			{
				if(area[i]>=(1.25*1000000)&&area[i]<=(1.4*1000000))
					banana++;
				else if(area[i]>(1.5*1000000)&&area[i]<(1.6*1000000))
					grapefruit++;
				else if(area[i]<=1000000&&area[i]>=700000)
					apple++;
			}			
			cout<<"\n Number of bananas = "<<banana;
			cout<<"\n Number of grapefruits = "<<grapefruit;
			cout<<"\n Number of apples = "<<apple<<endl;
			int z;
			x=y=z=1;
			for(int i=0;i<s;i++)
			{
				if(area[i]>=(1.25*1000000)&&area[i]<=(1.4*1000000))
				{	
					cout<<"\n Moments of Banana "<<x<<" are:"<<endl;
					cout<<" m00 = "<<m00[i]<<endl;
					cout<<" m10 = "<<m10[i]<<endl;
					cout<<" m01 = "<<m01[i]<<endl;
					cout<<" m20 = "<<m20[i]<<endl;
					cout<<" m02 = "<<m02[i]<<endl;
					cout<<" m11 = "<<m11[i]<<endl;
					cout<<" mu00 = "<<mu00[i]<<endl;
					cout<<" mu11 = "<<mu11[i]<<endl;
					cout<<" mu20 = "<<mu20[i]<<endl;
					cout<<" mu02 = "<<mu02[i]<<endl;
					cout<<" Compactness = "<<compactness[i]<<endl;
					cout<<" Eccentricity = "<<e[i]<<endl;
					cout<<" Direction angle = "<<direction[i]<<endl;
					x++;
				}

				else if(area[i]>(1.5*1000000)&&area[i]<(1.6*1000000))
				{
					cout<<"\n Moments of Grapefruit "<<y<<" are:"<<endl;
					cout<<" m00 = "<<m00[i]<<endl;
					cout<<" m10 = "<<m10[i]<<endl;
					cout<<" m01 = "<<m01[i]<<endl;
					cout<<" m20 = "<<m20[i]<<endl;
					cout<<" m02 = "<<m02[i]<<endl;
					cout<<" m11 = "<<m11[i]<<endl;
					cout<<" mu00 = "<<mu00[i]<<endl;
					cout<<" mu11 = "<<mu11[i]<<endl;
					cout<<" mu20 = "<<mu20[i]<<endl;
					cout<<" mu02 = "<<mu02[i]<<endl;
					cout<<" Compactness = "<<compactness[i]<<endl;
					cout<<" Eccentricity = "<<e[i]<<endl;
					cout<<" Direction angle = "<<direction[i]<<endl;
					y++;
				}
				else if(area[i]<=1000000&&area[i]>=700000)
				{
					cout<<"\n Moments of Apple "<<z<<" are:"<<endl;
					cout<<" m00 = "<<m00[i]<<endl;
					cout<<" m10 = "<<m10[i]<<endl;
					cout<<" m01 = "<<m01[i]<<endl;
					cout<<" m20 = "<<m20[i]<<endl;
					cout<<" m02 = "<<m02[i]<<endl;
					cout<<" m11 = "<<m11[i]<<endl;
					cout<<" mu00 = "<<mu00[i]<<endl;
					cout<<" mu11 = "<<mu11[i]<<endl;
					cout<<" mu20 = "<<mu20[i]<<endl;
					cout<<" mu02 = "<<mu02[i]<<endl;
					cout<<" Compactness = "<<compactness[i]<<endl;
					cout<<" Eccentricity = "<<e[i]<<endl;
					cout<<" Direction angle = "<<direction[i]<<endl;
					z++;
				}
			}

			Figure figtest;
			figtest.Draw(output);
		}	
		EventLoop();
	}
	catch(const Exception &p)
	{
		p.Display();
	}

	return 0;
}