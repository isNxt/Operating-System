#include <bits/stdc++.h>
using namespace std;
int a[]={2,3,2,1,5,2,4,5,3,2,5,2}; 
int b[3]={-1,-1,-1};  //只分配了三个块
 
void LRU(){
    int cnt = 0,kk = 0;
    memset(b,-1,sizeof(b));
    for(int i=0;i<12;i++){
        if(kk<3){ //前三个页调入内存
            bool flag = false;
            for(int j = 0;j < i ;j++){ //看看内存块中是否已经存在
                    if(a[i]==b[j]){
                        flag = true;
                        break;
                    }
            }
            if(flag==false){
            b[kk] = a[i];
            cnt++;
            kk++;
            }
        }
        else{
            bool flag = false;
            for(int j = 0;j < 3;j++){
                if(a[i]==b[j]){
                    flag = true;
                    break;
                }
            }
            //造成缺页
            if(flag == false){
                int tmp = 0;
                bool vis[3];
                memset(vis,false,sizeof(vis));
                for(int j = i;j >=0;j--){//向前找
                    for(int k=0;k<3;k++){
                        if(a[j]==b[k]){
                            vis[k]=true;
                            tmp++;
                            break;
                        }
                    }
                    if(tmp==2)
                        break;
                }
                for(int j = 0;j<3;j++){
                    if(vis[j]==false){
                        cnt++;
                        b[j]=a[i];
                        break;
                    }
                }
            }
        }
        cout<<a[i]<<": [";
        for(int j=0;j<3;j++){
            if(b[j]==-1)
                cout<<"* ";
            else
                cout<<b[j]<<" ";
        }
        cout<<"]"<<endl;
    }
    cout<<"缺页数为:  "<<cnt<<endl;
}
void OPT(){
   int cnt = 0,kk = 0;
    memset(b,-1,sizeof(b));
    for(int i=0;i<12;i++){
        if(kk<3){
            bool flag = false;
            for(int j = 0;j < i ;j++){
                    if(a[i]==b[j]){
                        flag = true;
                        break;
                    }
            }
            if(flag==false){
            b[kk] = a[i];
            cnt++;
            kk++;
            }
        }
        else{
            bool flag = false;
            for(int j = 0;j < 3;j++){
                if(a[i]==b[j]){
                        //cout<<"~~~~~~~"<<a[i]<<endl;
                    flag = true;
                    break;
                }
            }
            //造成缺页
            if(flag == false){
                int tmp = 0;
                bool vis[3];
                memset(vis,false,sizeof(vis));
                for(int j = i+1;j <12;j++){//向后找
                    for(int k=0;k<3;k++){
                        if(a[j]==b[k]){
                            vis[k]=true;
                            tmp++;
                            break;
                        }
                    }
                    if(tmp==2)
                        break;
                }
                for(int j = 0;j<3;j++){
                    if(vis[j]==false){
                        cnt++;
                        b[j]=a[i];
                        break;
                    }
                }
            }
        }
        cout<<a[i]<<": [";
        for(int j=0;j<3;j++){
            if(b[j]==-1)
                cout<<"* ";
            else
                cout<<b[j]<<" ";
        }
        cout<<"]"<<endl;
    }
    cout<<"缺页数为:  "<<cnt<<endl;
 
}
void FIFO(){
    int cnt = 0,t = 0;
    for(int i = 0;i <12 ;i++){
        bool flag = false;
        for(int j = 0;j < 3;j++){
            if(a[i]==b[j]){
                flag=true;
                break;
            }
        }
        if(flag==false){//缺页
            cnt++;
            b[t]=a[i];
            t++;
            if(t==3)
                t=0;
        }
        cout<<a[i]<<": [";
        for(int j=0;j<3;j++){
            if(b[j]==-1)
                cout<<"* ";
            else
                cout<<b[j]<<" ";
        }
        cout<<"]"<<endl;
    }
     cout<<"缺页数为:  "<<cnt<<endl;
}
int main(){
    cout<<"先进先出淘汰算法:"<<endl;
    cout<<"*********************"<<endl;
    FIFO();
    cout<<"*********************"<<endl;
    cout<<endl;
    cout<<"最近最久未用淘汰算法:"<<endl;
    cout<<"*********************"<<endl;
    LRU();
    cout<<"*********************"<<endl;
    cout<<endl;
     cout<<"最佳淘汰算法淘汰算法:"<<endl;
    cout<<"*********************"<<endl;
    OPT();
    cout<<"*********************"<<endl;
    cout<<endl;
 
    return 0;
}
