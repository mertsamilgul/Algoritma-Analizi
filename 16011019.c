#include <stdio.h>
#include <math.h>
#include <stdlib.h>
int part(double** p,int low,int high,int xy)
{
    double tmp,tmp1; // swap yapmak icin degiskenler
    int pivot=p[high][xy];
    int i=low-1,j;

    for(j=low;j<=high-1;j++)
    {
        if(p[j][xy]<pivot)
        {
            i++;
            tmp=p[i][0];
            tmp1=p[i][1];
            p[i][0]=p[j][0];
            p[i][1]=p[j][1];
            p[j][0]=tmp;
            p[j][1]=tmp1;
        }
    }
    tmp=p[i+1][0];
    tmp1=p[i+1][1];
    p[i+1][0]=p[high][0];
    p[i+1][1]=p[high][1];
    p[high][0]=tmp;
    p[high][1]=tmp1;
    return (i+1);
}
void quickxy(double** p, int low, int high,int xy) // hem x hem y koordinatlarina gore siralama yapabilen quickxysort fonksiyonu
{                                                  // xy degeri 0 oldugunda x e gore, 1 oldugunda y ye gore siralama yapar
    int pi;
    if (low<high)
    {
        pi=part(p,low,high,xy);
        quickxy(p,low,pi-1,xy);
        quickxy(p,pi+1,high,xy);
    }
}
double min(double x,double y) // verilen double degiskenlerinden kucuk olani donduren fonksiyon
{
    return x<y?x:y;
}
double distance(double** p,int i,int j,double* enyakin) // verilen iki indisteki noktalarin birbirine uzakligini bulan fonksiyon
{
    double uz=sqrt(pow(p[j][0]-p[i][0],2)+pow(p[j][1]-p[i][1],2));
    if(uz<enyakin[4])
    {
        enyakin[0]=p[i][0];
        enyakin[2]=p[j][0];
        enyakin[1]=p[i][1];
        enyakin[3]=p[j][1];
        enyakin[4]=uz;
    }
    return uz;
}
double closest(double** p,int i,int j,double* enyakin)
{
    double dl,dr,d;
    int mid,i2,j2,k;

    if(j-i<=2) // ise brute force
    {
        if(j-i==1)
            return distance(p,i,j,enyakin);
        return min(distance(p,i,j,enyakin),min(distance(p,i,j-1,enyakin),distance(p,j-1,j,enyakin)));
    }

    mid=((i+j)/2); // medyan noktanin indisi bulunur.
    dl=closest(p,i,mid,enyakin);
    dr=closest(p,mid+1,j,enyakin);
    d=min(dl,dr);

    i2=mid-1; // ozel durum icin medyanin sagina ve soluna bakilir.
    j2=mid+1;

    while(i2>=i && p[mid][0]-p[i2][0]<d)
        i2--;
    i2++;

    while(j2<=j && p[j2][0]-p[mid][0]<d)
        j2++;
    j2--;

    if(i2!=j2) // esitse bakilacak nokta yok
    {
        k=j2-i2+1;

        double **p2=(double**)malloc(sizeof(double*)*k);
        for(j=0;j<k;j++)
            p2[j]=(double*) malloc(sizeof(double)*2);

        for(j=0;j<k;j++,i2++)
        {
            p2[j][0]=p[i2][0]; // ozel duruma giren noktalar siralanmak icin baska bir diziye atilir.
            p2[j][1]=p[i2][1];
        }

        quickxy(p2,0,k-1,1); // y'ye gore siralama yapilir

        for(i=0;i<k-1;i++)
            for(j=i+1;j<k && p2[j][1]-p2[i][1]<d;j++)
                if(distance(p2,i,j,enyakin)<d)
                    d=distance(p2,i,j,enyakin);

        for(j=0;j<k;j++)
            free(p2[j]);
    }
    return d;
}
void bul(double** p,int n)
{
    double enyakin[5]={p[0][0],p[0][1],p[1][0],p[1][1],sqrt(pow(p[0][0]-p[1][0],2)+pow(p[0][1]-p[1][1],2))};
    // En yakin noktalarin degerlerinin ve uzakliginin tutulacagi matris
    double minuzaklik=closest(p,0,n-1,enyakin);
    printf("En yakin noktalar: (%lf,%lf) ve (%lf,%lf)\n",enyakin[0],enyakin[1],enyakin[2],enyakin[3]);
    printf("Uzakliklari: %lf",minuzaklik);
}
int main()
{
    FILE *fp;
    int i,n=0;
    double d1,d2;
    fp=fopen("input.txt","r");

    while(!feof(fp))
    {
        if(getc(fp)=='\n') // kac tane nokta oldugu bulunur.
            n++;
    }
    n++; // son satir icin (txt dosyasinda son elemandan sonra newline koyulmamalidir).

    double **p=(double**)malloc(sizeof(double*)*n);  // noktalar icin yer acilir.
        for(i=0;i<n;i++)
            p[i]=(double*) malloc(sizeof(double)*2);

    fseek(fp,0,SEEK_SET); // noktalari almak icin dosya tekrar okunacagindan dosyanin basina gidilir.

    for(i=0;i<n;i++)
    {
        fscanf(fp,"%lf %lf",&p[i][0],&p[i][1]); // noktalar dosyadan alinir.
    }

    printf("Noktalar: \n");

    for(i=0;i<n;i++)
    {
        printf("(%lf,%lf)\n",p[i][0],p[i][1]);
    }

    quickxy(p,0,n-1,0); // x'e gore siralama.

    bul(p,n);

    fclose(fp);

    return 0;
}
