#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <fstream>
#include <vector>
using namespace std;
string inttostr(int a){ // Permet de convertir un entier en string, utile pour sauvegarder
    return std::to_string(a);
}
int strtoint(string a){ //Permet de convertir un string en entier, utile pour charger la sauvegarde
    return std::stoi(a);
}
time_t convert(int jour, int mois, int annee){ //Permet de convertir la date rentrée par l'utilisateur en time_t
    time_t date = time(0);
    tm * t = localtime(&date);
    t->tm_mday = jour;
    t->tm_mon = mois-1;
    t->tm_year = annee - 1900;
    t->tm_hour = 1;
    t->tm_min =0;
    t->tm_sec = 0; 
    date = mktime(t); // passe de la structure tm au type time_t. 
    //Le passage par tm permet de modifier plus facilement date, jour et année car time_t correspond à un entier du nombre de secondes depuis 1900.
    return date;
}
string codage(string phrase){ //Permet de remplacer les espace par _ car dans la sauvegarde les différentes info sont séparé par des espaces
    for (int i = 0; i<phrase.size();i++){
        char esp = ' ';
        char tir = '_';
        if (phrase[i] == esp){
            phrase[i] = tir;
        }
    }
    return phrase;
}
string decodage(string code){ //inverse de codage pour pouvoir lire une sauvegarde
    for (int i = 0; i<code.size();i++){
        char esp = ' ';
        char tir = '_';
        if (code[i] == tir){
            code[i] = esp;
        }
    }
    return code;

}
vector<string> decoupe(string ligne){ //Sépare les mots d'une string. Utile pour lire une ligne du fichier de sauvegarde.
    vector<string> liste;
    liste.push_back("");
    int a = 0;
    for (int i(0); i<ligne.size() ;i++){
        char mot =' ';
        if (ligne[i] == mot){
            while(((i+1)<ligne.size()) and (ligne[i+1]==mot)){ // gérer le cas avec 2 espaces pour l'utilisation de decoupe dans l'affichage de la date
                i=i+1;
            }
            a=a+1;
            liste.push_back("");
        }
        else{
            liste[a] = liste[a] + ligne[i];
        }
    }
    return liste;
}

class comment{ //Classe commentaire 
friend class Task;
string comm;
public :
    comment(string text) : comm(text){}
    string text(){
        return comm;
    }
};

class Task { // Classe Taches
private :
    
    string title;
    string descrip;
    time_t datedeb;
    time_t datefin;
    string status;
    int avanc;
    string prio;
    vector<comment> comm;
    vector<string> sous_task; //Pas encore developper donc inutile pour l'instant
    string id;
public :
//Construceur, dans le cas d'une création de l'utilisateur
    Task(string a,string b, time_t deb, time_t fin, string prio) : title(a),descrip(b), datedeb(deb), datefin(fin) ,prio(prio) {
        status = "ouverte";
        avanc = 0;
        int nbr = rand() % 10;
        id = inttostr(nbr) + codage(title)[0] + codage(title)[1] + codage(title)[2]+ codage(prio)[0] +  codage(prio)[1];
    }
    //Constructeur dans le cas d'une sauvegarde de destruction
    Task(string c,string a,string b, time_t deb, time_t fin, string prio) : id(c), title(a),descrip(b), datedeb(deb), datefin(fin) ,prio(prio){}
    void sauvegarde(){ // Ecris une ligne dans le fichier de sauvegarde pour sauvegarder la taches lorsque le programme ferme
        ofstream taches("taches.txt", ios::app);
        if(taches){
        taches.seekp(0, ios::end);
        
        taches << id << " " << codage(title) << " " << codage(descrip) << " " << datedeb << " " << datefin << " " << codage(prio);
        for (int i =0;i<comm.size();i++){
            taches << " " << codage(comm[i].text());
        }
        taches << endl;
        }
        else{
        cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
        }
    }
    void afficher(){ //Permet d'afficher une tâche
        vector<string> ldeb= decoupe(ctime(&datedeb)); //j'ai essayer d'utiliser localtime(&datedeb) mais cela ne fonctionnait pas du tout
        vector<string> lfin = decoupe(ctime(&datefin));
        time_t actuel = time(0);
        if(actuel<datedeb){
            status = "ouverte";
            avanc = 0;
        }
        if((datedeb<=actuel) and (actuel<datefin+86400)){
            status  = "en progression";
            double a = actuel-datedeb;
            double b = datefin+86400-datedeb;
            avanc = (a/b)*100;
        }
        if(datefin+864000<=actuel){
            status = "fermee";
            avanc = 100;
        }
        cout << "Titre : " << title << endl;
        cout << "Description : "<< descrip<<endl;
        cout << "Date début : " << ldeb[2] <<" "<<ldeb[1]<<" "<<ldeb[4]<<endl;
        cout << "Date fin : " << lfin[2] <<" "<< lfin[1] <<" "<< lfin[4] <<endl;
        cout << "Status : " << status << endl;
        cout << "Avancement : " << avanc << "%" << endl;
        cout << "Priorité : " << prio << endl;
        if (comm.size()>0){
            cout << "Comment :"<<endl;
            for (int d=0;d<comm.size();d++){
                cout << d<<") " << comm[d].text()<< endl;
            }
        }
    }
    string priorite(){ //permet d'avoir accès à priorité pour l'afficher par exmple
        return prio;
    }
    string titre(){ //permet d'avoir accès à titre pour l'afficher par exmple
        return title;
    }
    void modifier(time_t deb, time_t fin){ //Modification de la date
        datedeb =deb;
        datefin = fin;
    }
    void ajouter(string text){ //ajout d'un commentaire
        comm.push_back(comment(text));
    }
    
};

Task charge(string ligne){ //Permet de créer une nouvelle tâches lors de la lecture de la ligne de sauvegarde ligne.
    vector<string> b;
    b = decoupe(ligne);
    time_t jourdeb = strtoint(b[3]);
    time_t jourfin = strtoint(b[4]);
    Task nvtask = Task(b[0],decodage(b[1]),decodage(b[2]),jourdeb,jourfin,decodage(b[5]));
    for (int i =6;i<b.size();i++){
        nvtask.ajouter(decodage(b[i]));
    }
    return(nvtask);
}
vector<Task> init(){ // Permet de lire le fichier de sauvegarde pour créer toutes les tâches en mémoire.
    ifstream taches("taches.txt");
    vector<Task> all_task;
    if (taches){
        
        taches.seekg(0, ios::beg);
        string ligne;
        while(getline(taches,ligne)){
            if (ligne != ""){
            all_task.push_back(charge(ligne));
            }
        }
    }
    else{//dans le cas où on n'arrive pas à lire le fichier
        cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
    }
    
    return all_task;
}
vector<Task> list(vector<Task> all_task,string a){ // Permet de lister les taches à l'utilisateur en fonction de leur priorité.
    vector<Task> list;
    int count = 0;
    for (int i = 0;i<all_task.size();i++){

        if ((all_task[i].priorite() == a )or(a=="All") ){
            cout << count+1 << ") " << all_task[i].titre() << endl;
            list.push_back(all_task[i]);
            count = count + 1;
        }
        }
    return list;
        
}
void destruction(vector<Task> all_task){ // sauvegarde des fichiers à la fin de l'utilisation.
    ofstream taches ("taches.txt"); // efface le fichier sauvegarde pour réécrire une sauvegarde
    taches.close();
    for (int i=0;i<all_task.size();i++){
        all_task[i].sauvegarde();
    }
}

int main(){
    vector<Task> all_task = init(); // commence par créer une liste de toutes les taches en mémoire
    string action = "a";
    while (action != "quit"){ // demande à l'utilisateur ce qu'il veut faire
    cout << "What do you want to do : create, modify, list, delete or quit ?" << endl;
    getline(cin,action);
    if (action == "create"){ // dans le cas de la création, demande toutes les informations nécessaires à la création d'une tâche
        string title="";
        int pri;
        string descrip="";
        int anneed, moisd, jourd,anneef,moisf,jourf;
        cout << "Title : ";
        getline(cin,title);
        cout << "Description : ";
        getline(cin,descrip);
        cout << "Priority : 1) High, 2) Medium, 3)Low " <<endl << "Enter number :"<<endl;
        cin>>pri;
        
        cout << "Begining date : " << endl;
        cout << "Day : ";
        cin >> jourd;
        cout << "Mois : ";
        cin >> moisd;
        cout << "Année : ";
        cin >> anneed;
        cout << "Ending date : " << endl;
        cout << "Day : ";
        cin >> jourf;
        cout << "Mois : ";
        cin >> moisf;
        cout << "Année : ";
        cin >> anneef;
        vector<string> diction;
        diction.push_back("All"); diction.push_back("High"); diction.push_back("Medium");diction.push_back("Low;");
        time_t datedeb = convert(jourd,moisd,anneed);
        time_t datefin = convert(jourf,moisf,anneef);
        Task nvtaches = Task(title,descrip,datedeb,datefin,diction[pri]);
        all_task.push_back(nvtaches); // créer la tâche
    }
    if (action =="list"){ //liste les taches.
        cout <<"1) All"<<endl;
        cout <<"2) High"<<endl;
        cout <<"3) Medium"<<endl;
        cout <<"4) Low"<<endl;
        cout << "Which priority do you want to see ? (number)"<< endl;
        int a;
        cin >> a;
        vector<string> diction;
        diction.push_back("All"); diction.push_back("High"); diction.push_back("Medium");diction.push_back("Low;");
        vector<Task> new_list;
        new_list = list(all_task,diction[a-1]);
        cout << "Which one would you like to see ? (number)" << endl;
        int nombre;
        cin >> nombre;
        new_list[nombre-1].afficher();

    }
    if (action =="delete"){ //supprime une tache.
        cout << "0) None" << endl;
        vector<Task> new_list;
        new_list = list(all_task,"All");
        int a =0;
        cout << "Which task will you like to delete ? (number)" << endl;
        cin >> a;
        if (a!=0){
            for (int i = a-1;i<all_task.size()-1;i++){
                all_task[i] = all_task[i+1]; // je fais ça car le ".erase()" qui fonctionne sur les vector ne fonctionne pas sur les vector de Task car il n'y a pas de destructeur.
            
            }
            all_task.pop_back();
        }
    }
    if (action == "modify"){ // modifie une tâches (ne peut que modifier la date ou ajouter un commentaire)
        cout << "0) None" << endl;
        list(all_task,"All");
        string bonus;
        int a =0;
        cout << "Which task will you like to modify ? (number)" << endl;
        getline(cin,bonus); // je suis obligé de faire ça car le "cin >> a" pose problème avec les espaces, il arrive quela réponse à la question d'après saute.
        a = strtoint(bonus);
        cout << "Modify information or add comment ? (m or a) "<<endl;
        string text;
        getline(cin,text);
        if (text == "m"){
        int anneed, moisd, jourd,anneef,moisf,jourf;
        cout << "Begining date : " << endl;
        cout << "Day : ";
        cin >> jourd;
        cout << "Mois : ";
        cin >> moisd;
        cout << "Année : ";
        cin >> anneed;
        cout << "Ending date : " << endl;
        cout << "Day : ";
        cin >> jourf;
        cout << "Mois : ";
        cin >> moisf;
        cout << "Année : ";
        cin >> anneef;
        time_t datedeb = convert(jourd,moisd,anneed);
        time_t datefin = convert(jourf,moisf,anneef);
        all_task[a-1].modifier(datedeb,datefin);
        }
        if (text=="a"){
            string commentaire;
            cout << "Comment : ";
            getline(cin,commentaire);
            all_task[a-1].ajouter(commentaire);
        }
    }
    }

    destruction(all_task); //lorsqu'on sort du while, on détruit la liste de tâche en la réecrivant sur le fichier de sauvegarde.
    return 0;
}