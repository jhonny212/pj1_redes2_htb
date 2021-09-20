#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;
vector<string> split(string str, char pattern, bool enter);
vector<vector<string>> enlace;
vector<vector<string>> modo;
vector<vector<string>> proto;
vector<vector<string>> usuario;
vector<vector<string>> macusuario;
bool isdinamic;

void abrir();
void create0();
void create1();
void create2();
void create3();
void modalidad1();
void modalidad2();
void protocolos();

FILE * archivo;


long medida;
char * textoenlace;
char * textomodo;
char * textoproto;
char * textousuario;
string archivocrontab;

int counter;
int id;
int upband=0;
int downband=0;
int main()
{
    //	
  
    isdinamic=false;
    id=0;
    counter=0;
    create0();
    create1();
    create2();
    create3();
    modalidad1();
    protocolos();
    return 0;
}

//crear array de enlace
void create0(){
    abrir();
    vector<string> res=split(textoenlace,'\n',false);
    for(int i=0; i<res.size(); i++){
        enlace.push_back(split(res[i],'=',false));
    }

    for(int i=0; i<enlace.size(); i++){
        vector<string>tmp=enlace[i];
         if(tmp.size()!=2){break; }
        if(tmp[0]=="up"){
            upband=stoi(tmp[1]);
        }else if(tmp[0]=="down"){
            downband=stoi(tmp[1]);
        }
    }
    cout<<"velocidad de subida:"<<upband<<endl;
    cout<<"velocidad de bajada:"<<downband<<endl;
    
    
}

//crear array de modo
void create1(){
    abrir();
    vector<string> res=split(textomodo,'\n',false);
    vector<string> modo=split(textomodo,'=',false);
    if (modo[1]=="1\n") {
    	isdinamic=true;
    	
    }
    if (modo[1]=="1") {
    	isdinamic=true;
    	
    }
    cout<<"es dinamico:"<<isdinamic<<endl;

}

//crear array de usuario
void create2(){
    abrir();
    vector<string> res=split(textousuario,'\n',false);
    for(int i=0; i<res.size(); i++){
        usuario.push_back(split(res[i],',',false));
    }


}

//crear array de protocolo
void create3(){
    abrir();
    vector<string> res=split(textoproto,'\n',false);
    for(int i=0; i<res.size(); i++){
        proto.push_back(split(res[i],',',false));
    }

}

//abrir archivos y almacenarlos
void abrir(){

    if (counter==0){
        archivo = fopen ("enlace.conf", "r");
    }
    if (counter==1){
        archivo= fopen ("modo.conf", "r");
    }
    if (counter==2){
        archivo = fopen ("usuario_BW.conf", "r");
    }

    if (counter==3){
        archivo= fopen ("usuario_Proto.conf", "r");
    }

    fseek (archivo , 0 , SEEK_END);
    medida = ftell (archivo);
    rewind (archivo);
    if (counter==0){
        textoenlace = (char*) malloc (sizeof(char)*medida);
        fread(textoenlace, medida+1, 1, archivo);

    }
    if (counter==1){
        textomodo = (char*) malloc (sizeof(char)*medida);
        fread(textomodo, medida+1, 1, archivo);

    }
    if (counter==2){
        textousuario = (char*) malloc (sizeof(char)*medida);
        fread(textousuario, medida+1, 1, archivo);

    }

    if (counter==3){
        textoproto = (char*) malloc (sizeof(char)*medida);
        fread(textoproto, medida+1, 1, archivo);

    }

    fclose(archivo);
    counter=counter+1;
}

vector<string> split(string str, char pattern,bool enter) {
    int posInit = 0;
    int posFound = 0;
    string splitted;
    vector<string> resultados;
    char chars[]="(\n\t";
    while(posFound >= 0){
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;

        if(enter){
            for (unsigned int i = 0; i < 2; ++i) {
            splitted.erase (std::remove(splitted.begin(), splitted.end(), chars[i]), splitted.end());
            }
        }

        resultados.push_back(splitted);

    }


    return resultados;
}


void modalidad1(){
    //eliminar enlaces	
    
    //system("/usr/sbin/tc qdisc del dev enp7s0 root");
    
    system("sudo insmod sch_htb 2> /dev/null");
    //crear raiz
    system("/usr/sbin/tc qdisc add dev enp7s0 root       handle 1:    htb default 0xA");
    archivocrontab="";
    for(int i=0; i<usuario.size(); i++){
        vector<string>tmp=usuario[i];
        vector<string>mac=split(tmp[0],':',false);
        //crear enlaces id
        int id=i+20;
        string command="/usr/sbin/tc class add dev enp7s0 parent 1: classid 1:"+to_string(id);
        if(tmp.size()!=5){break; }
      
        
        string strdown=tmp[1];
        
        string strup=tmp[2];
        int intup=(stoi(strup)*(10.24))*upband;
        int intdown=(stoi(strdown)*(10.24))*(downband);
        intdown=intdown+intup;
        int intdefault=1;
        
        cout<<"ancho de banda a asignar a la mac "<<tmp[0]<<" "<<intdown<<endl;
        command=command+" htb rate "+to_string(intdefault)+"kbit ceil "+to_string(intdefault)+"kbit";
        
        //crear reglase del enlace
        cout<<command<<endl;
        system(command.c_str());
        string M2= mac[4]+mac[5];
        string M0= mac[0]+mac[1];
        string M1= mac[2]+mac[3];
	string TCF="/usr/sbin/tc filter add dev enp7s0 parent 1: protocol ip prio 5 u32 match u16 0x0800 0xFFFF at -2";
        string filter_by_mac1=TCF+" match u16 0x"+M2+" 0xFFFF at -4 match u32 0x"+M0+M1;
        filter_by_mac1=filter_by_mac1+" 0xFFFFFFFF at -8 flowid 1:"+to_string(id);

        string filter_by_mac2=TCF+" match u32 0x"+M1+M2+" 0xFFFFFFFF at -12 match u16 0x"+M0;
        filter_by_mac2=filter_by_mac2+" 0xFFFF at -14 flowid 1:"+to_string(id);
       //asignar por mac
        cout<<filter_by_mac1<<endl;
        cout<<filter_by_mac2<<endl;
        
        system(filter_by_mac1.c_str());
        system(filter_by_mac2.c_str());
        ///////
        vector<string> init=split(tmp[3],':',false);
        cout<<"Hora inicio: "<<init[0]<<":"<<init[1];
        archivocrontab=archivocrontab+init[1]+" "+init[0]+" * * * /sbin/tc class change dev";
        if(isdinamic){
        archivocrontab=archivocrontab+" enp7s0 parent 1:1 classid 1:"+to_string(id)+" htb rate "+to_string(intdown)+"kbit \n";
        }else{
        archivocrontab=archivocrontab+" enp7s0 parent 1:1 classid 1:"+to_string(id)+" htb rate "+to_string(intdown)+"kbit ceil "+to_string(intdown)+"kbit\n";
        }
        
        vector<string> end=split(tmp[4],':',false);
        cout<<" Hora fin: "<<end[0]<<":"<<end[1]<<endl;
        archivocrontab=archivocrontab+end[1]+" "+end[0]+" * * * /sbin/tc class change dev";
        archivocrontab=archivocrontab+" enp7s0 parent 1:1 classid 1:"+to_string(id)+" htb rate "+to_string(intdefault)+"kbit ceil "+to_string(intdefault)+"kbit\n";
        
    }

}

void protocolos() {
    
    for(int i=0; i<proto.size(); i++){
        vector<string>tmp=proto[i];
        if(tmp.size()!=5){break; }
    	string time=" -m time --timestart "+tmp[3]+" --timestop "+tmp[4]+"";
    	vector<string> init=split(tmp[3],':',false);
    	vector<string> end=split(tmp[4],':',false);
    	string timeini=init[1]+" "+init[0]+" * * * ";
    	string timeend=end[1]+" "+end[0]+" * * * ";
    	
        string command="";
        
        cout<<"Aplicando protocolo a "<<tmp[0]<<endl;
    	if(tmp[1]=="udp"){
    	command=timeini+"/sbin/iptables -I FORWARD 1 -p udp -m mac --mac-source "+tmp[0]+" -m udp --dport "+tmp[2]+" -j ACCEPT";
    	archivocrontab=archivocrontab+command+"\n";
    	cout<<command<<endl;
    	command=timeini+"/sbin/iptables -I FORWARD 1 -p udp -m state --state RELATED,ESTABLISHED -m udp --sport "+tmp[2]+" -j ACCEPT";
    	cout<<command<<endl;
    	archivocrontab=archivocrontab+command+"\n";
//fin
    	command=timeend+"/sbin/iptables -D FORWARD -p udp -m mac --mac-source "+tmp[0]+" -m udp --dport "+tmp[2]+" -j ACCEPT";
    	archivocrontab=archivocrontab+command+"\n";
    	cout<<command<<endl;
    	command=timeend+"/sbin/iptables -D FORWARD -p udp -m state --state RELATED,ESTABLISHED -m udp --sport "+tmp[2]+" -j ACCEPT";
    	cout<<command<<endl;
    	archivocrontab=archivocrontab+command+"\n";
    	
    	
    	}else if(tmp[1]=="tcp"){
    	command=timeini+"/sbin/iptables -I FORWARD 1 -p tcp -m mac --mac-source "+tmp[0]+" -m tcp --dport "+tmp[2]+" -j ACCEPT";
    	cout<<command<<endl;
    	archivocrontab=archivocrontab+command+"\n";
    	command=timeini+"/sbin/iptables -I FORWARD 1 -p tcp -m state --state RELATED,ESTABLISHED -m tcp --sport "+tmp[2]+" -j ACCEPT";
    	cout<<command<<endl;
    	archivocrontab=archivocrontab+command+"\n";
//fin
    	command=timeend+"/sbin/iptables -D FORWARD -p tcp -m mac --mac-source "+tmp[0]+" -m tcp --dport "+tmp[2]+" -j ACCEPT";
    	cout<<command<<endl;
    	archivocrontab=archivocrontab+command+"\n";
    	command=timeend+"/sbin/iptables -D FORWARD -p tcp -m state --state RELATED,ESTABLISHED -m tcp --sport "+tmp[2]+" -j ACCEPT";
    	cout<<command<<endl;
    	archivocrontab=archivocrontab+command+"\n";
    	    	
    	}else if(tmp[1]=="icmp"){
    	command=timeini+"/sbin/iptables -I FORWARD 1 -p icmp -m mac --mac-source "+tmp[0]+" -j ACCEPT";
    	archivocrontab=archivocrontab+command+"\n";
    	cout<<command<<endl;
    	command=timeini+"/sbin/iptables -I FORWARD 1 -p icmp -m state --state RELATED,ESTABLISHED"+" -j ACCEPT";
    	archivocrontab=archivocrontab+command+"\n";
    	cout<<command<<endl;
    	//fin
    	command=timeend+"/sbin/iptables -D FORWARD -p icmp -m mac --mac-source "+tmp[0]+" -j ACCEPT";
    	archivocrontab=archivocrontab+command+"\n";
    	cout<<command<<endl;
    	command=timeend+"/sbin/iptables -D FORWARD -p icmp -m state --state RELATED,ESTABLISHED"+" -j ACCEPT";
    	archivocrontab=archivocrontab+command+"\n";
    	cout<<command<<endl;
    	}
      
        
    }
    system("crontab /home/jhonny/Desktop/archivocontrab");
    ofstream file;
    file.open("/home/jhonny/Desktop/archivocontrab");
    file << archivocrontab.c_str();
    file.close();
    system("sudo crontab -u root /home/jhonny/Desktop/archivocontrab");
}
