#include <iostream>
#include <windows.h>
#include <mysql.h>
#include <iomanip>
#include <string>
#include <bits/stdc++.h>
#include <ctime>

//#include <my_global.h>
MYSQL* conn;

MYSQL_ROW row;
MYSQL_RES *res;
using namespace std;
class Complaint
{
private:
    set<string> key1={"fan","electrical","ac","light","charging","port"};
    set<string> key2={"water","clean","flush","leakage","spill","spoil"};
    set<string> key3={"biscuits","bottle","oil","food","catering","meal","rice","salt","sugar","vada","dosa","idlli","sambar"};
    int Department_Pointer[10],D_Pointer[10], Display_Pointer=0,Display_Limit=0;
    struct Department
    {
        /**
        S.No Department:
        1       Electrical
        2       Sanitary
        3
        **/
        int id;
        string Name;
        string Phone_number;
        string complaint;
        string pnr;
        string Train_number;
        string coach_number;
        string status;
        string date;
        struct Department *prev,*next;
    }*D[10],*D_Temp[10],*D_Head[10],*D_Tail[10];

    struct Record
    {
        int id;
        string Name;
        string Phone_number;
        string complaint;
        string pnr;
        string Train_number;
        string coach_number;
        string status;
        string date;
        struct Record *prev,*next;
    }*R=NULL,*Temp=NULL,*Head=NULL,*Tail=NULL;

    bool if_administrator=false, log_out=true, Empty=true;
    bool closed_complaints=false;
    bool if_registered=false;
    string Name,Mobile;
    int Name_length=6;
    int Complaint_length=11;

public:
    void Get_complaint();
    void Display(string,string);
    void Get_Data();
    void Update();
    void Login();
    void Logout();
    void Sort_Department();
    void Sort_Department_Sub(int,string,string,string,string,string,string,string,string,int);
    void Sort_Department_Display();
    void Initial();
    void Initial_pointer();
    void Display_Profile();
    void Register_Account();
    void Sort_Train();
    void Activity();

    void Convert_list(int,string,string,string,string,string,string,string,string);
    int Connect_sql();
    void Get_choice();
    void Execute(int);
    void Display_heading();
};
int main()
{
    Complaint C;

    C.Initial();
    C.Initial_pointer();
    int conn;
    conn=C.Connect_sql();
    if(conn!=0)
    {
        cout<<setw(50)<<right<<"Railway Complaint Management"<<endl;
        C.Get_choice();
    }

}
void Complaint::Initial()
{
    Display_Pointer=0,Display_Limit=0;
    for(int i=0;i<10;i++)
    {
        D[i]=NULL;
        D_Temp[i]=NULL;
        D_Head[i]=NULL;
        D_Tail[i]=NULL;
        Department_Pointer[i]=-1;
    }
}
void Complaint::Initial_pointer()
{
    for(int i=0;i<10;i++)
    {
        D_Pointer[i]=-1;
    }
}
void Complaint::Get_complaint()
{
    string pnr,status,Train_number,coach_number,complaint;
    string d,month,year,date;
    // current date/time based on current system
    time_t now = time(0);

    tm *ltm = localtime(&now);
    int Year,Month,Date;
    // print various components of tm structure.
    Year=1900 + ltm->tm_year;
    Month=1 + ltm->tm_mon;
    Date=ltm->tm_mday;

    cout<<setw(30)<<right<<"Detail\n";
    cout<<"Enter pnr: ";
    getline(cin, pnr);
    if(pnr=="")
        getline(cin,pnr);
    if(pnr.length()==10)
    {
        cout<<"Train Number: ";
        getline(cin,Train_number);
        if(Train_number=="")
            getline(cin,Train_number);
        cout<<"Coach Number: ";
        getline(cin,coach_number);
        if(coach_number=="")
            getline(cin,coach_number);
        status="Pending";
        cout<<"Complaint: ";
        getline(cin,complaint);
        if(complaint=="")
            getline(cin,complaint);
        if(complaint=="")
            return;
        d=to_string(Date), month=to_string(Month), year=to_string(Year);
        date=d+'-'+month+'-'+year;
        string query;
        int qstate;

        query = "INSERT INTO `complaint` (`id`,`pnr_code`, `train_number`, `coach_number`, `name`, `phone_number`, `date`,`complaint`,`status`) VALUES (0,'"+ pnr + "','" + Train_number + "','" + coach_number + "','"
        + Name + "','"+ Mobile + "','" + date + "','" + complaint + "','" + status + "');";///'+' - concatenation operator

        const char* q = query.c_str();

        qstate = mysql_query(conn, q);

        if(!qstate)
            cout<<"Complaint Registered"<<endl;
        else
            cout<<"query problem: "<<mysql_error(conn)<<endl;
    }
}
void Complaint::Get_Data()
{
    //cout<<"Display limit: "<<Display_Limit<<endl<<"Display Pointer: "<<Display_Pointer<<endl;
    ///cout<<"Opening my sql table\n";
    int qstate;
    string Name;
    qstate = mysql_query(conn,"select * from complaint");
    if(!qstate)
    {
        res = mysql_store_result(conn);
        int l_n,l_c;
        while(row = mysql_fetch_row(res))///For adjusting Table width
        {
            l_n=((string)row[4]).length()+2;
            if(Name_length<l_n)
                Name_length=l_n;
            l_c=((string)row[7]).length()+2;
            if(Complaint_length<l_c)
                Complaint_length=l_c;
        }
        //cout<<"Highest: "<<l_n<<endl;
        row=0;
        qstate = mysql_query(conn,"select * from complaint");
        res = mysql_store_result(conn);
        Display_Pointer=1;
        while(row = mysql_fetch_row(res))
        {
            ///cout<<"Display limit: "<<Display_Limit<<endl<<"Display Pointer: "<<Display_Pointer<<endl<<endl;
            if(Display_Pointer>Display_Limit)
            {
                Convert_list((int)row[0],(string)row[1],(string)row[2],(string)row[3],(string)row[4],(string)row[5],(string)row[6],(string)row[7],(string)row[8]);
                Display_Limit=Display_Limit+1;
            }
            Display_Pointer=Display_Pointer+1;
        }
        //cout<<"Data converted\n";
    }
    else
        cout<<"No data\n";
}
void Complaint::Sort_Department()
{
    R=Head;
    int length=0,l;
    string str,comp;
    Initial_pointer();
    while(R!=NULL)
    {
        length=1;
        R->complaint=R->complaint+" ";
        for(int i = 0; R->complaint[i] != '\0';i++)
        {
            if(R->complaint[i] == ' ')///Detects empty space
            {
                l=comp.length();
                for(int i=0;i<l; i++)
                    comp[i]=tolower(comp[i]);
                if(key1.find(comp)!=key1.end())
                {
                    D_Pointer[0]=D_Pointer[0]+1;
                    if(Department_Pointer[0]<D_Pointer[0])
                    {
                        Sort_Department_Sub(R->id,R->pnr,R->Train_number,R->coach_number,R->Name,R->Phone_number,R->date,R->complaint,R->status,0);
                        Department_Pointer[0]=Department_Pointer[0]+1;
                    }
                }
                else if(key2.find(comp)!=key2.end())
                {
                    D_Pointer[1]=D_Pointer[1]+1;
                    if(Department_Pointer[1]<D_Pointer[1])
                    {
                        Sort_Department_Sub(R->id,R->pnr,R->Train_number,R->coach_number,R->Name,R->Phone_number,R->date,R->complaint,R->status,1);
                        Department_Pointer[1]=Department_Pointer[1]+1;
                    }
                }
                else if(key3.find(comp)!=key3.end())
                {
                    D_Pointer[2]=D_Pointer[2]+1;

                    if(Department_Pointer[2]<D_Pointer[2])
                    {
                        Sort_Department_Sub(R->id,R->pnr,R->Train_number,R->coach_number,R->Name,R->Phone_number,R->date,R->complaint,R->status,2);
                        Department_Pointer[2]=Department_Pointer[2]+1;
                    }
                }
                length=length+1;
                comp="";
            }
            else
            {
                comp=comp+R->complaint[i];
            }
        }
        comp="";
        R=R->next;
    }
    Sort_Department_Display();
}
void Complaint::Sort_Department_Sub(int id,string pnr,string Train_no,string Coach_no,string Name,string Phone_no,string date,string complaint,string status,int type)
{
    D_Temp[type]=new struct Department;
    D_Temp[type]->next=NULL;
    D_Temp[type]->id=id;
    D_Temp[type]->pnr=pnr;
    D_Temp[type]->Train_number=Train_no;
    D_Temp[type]->coach_number=Coach_no;
    D_Temp[type]->Name=Name;
    D_Temp[type]->Phone_number=Phone_no;
    D_Temp[type]->date=date;
    D_Temp[type]->complaint=complaint;
    D_Temp[type]->status=status;
    if(D_Head[type]==NULL)
    {
        D_Head[type]=D_Temp[type];
        D_Tail[type]=D_Head[type];
        return;
    }
    D[type]=D_Head[type];
    while(D[type]->next!=NULL)
    {
        D[type]=D[type]->next;
    }
    D[type]->next=D_Temp[type];
    D_Temp[type]->prev=D[type];
    D_Tail[type]=D_Temp[type];
}
void Complaint::Sort_Train()
{
    string Train_Number;
    cout<<"Train Number: ";
    getline(cin,Train_Number);
    if(Train_Number=="")
        getline(cin,Train_Number);
    if(Train_Number.length()!=5)
        return;
    R=Head;
    int l_n,l_c;
    Name_length=6,Complaint_length=11;
    Empty=true;
    while(R!=NULL)
    {
        if((if_administrator)&&(R->Train_number==Train_Number)&&(R->status=="Pending"))
        {
            l_n=R->Name.length()+2;
            if(Name_length<l_n)
                Name_length=l_n;
            l_c=R->complaint.length()+2;
            if(Complaint_length<l_c)
                Complaint_length=l_c;
            Empty=false;
        }
        R=R->next;
    }
    R=Head;
    if(Empty==false)
    {
        Display_heading();
        while(R!=NULL)
        {
            if((if_administrator)&&(Empty==false)&&(R->Train_number==Train_Number&&(R->status=="Pending")))
            {
                cout<<setw(12)<<left<<R->pnr<<setw(14)<<left<<R->Train_number<<setw(14)<<left<<R->coach_number;
                cout<<setw(Name_length)<<left<<R->Name<<setw(14)<<left<<R->Phone_number<<setw(12)<<left<<R->date;
                cout<<setw(Complaint_length)<<left<<R->complaint<<setw(10)<<left<<R->status<<"\n";
            }
            R=R->next;
        }
    }
    else
    {
        cout<<"No record found\n";
    }
}
void Complaint::Sort_Department_Display()
{
    int ch;
    cout<<"\nChoose Department:\n1.Electrical Department\n2.Sanitary Department\n3.Food Department\n";
    cin>>ch;
    ch=ch-1;
    D[ch]=D_Head[ch];
    int l_n,l_c;
    Name_length=6,Complaint_length=11;
    Empty=true;
    while(D[ch]!=NULL)
    {
        if((if_administrator) || ((if_administrator==false) && (D[ch]->status=="Pending") && (D[ch]->Name==Name) && (D[ch]->Phone_number==Mobile)))
        {
            l_n=D[ch]->Name.length()+2;
            if(Name_length<l_n)
                Name_length=l_n;
            l_c=D[ch]->complaint.length()+2;
            if(Complaint_length<l_c)
                Complaint_length=l_c;
            Empty=false;
        }
        D[ch]=D[ch]->next;
    }
    D[ch]=D_Head[ch];
    if(Empty==false)
    {
        Display_heading();
        while(D[ch]!=NULL)
        {
            if((if_administrator) || ((if_administrator==false)  && (D[ch]->status=="Pending") && (D[ch]->Name==Name) && (D[ch]->Phone_number==Mobile)))
            {
                cout<<setw(12)<<left<<D[ch]->pnr<<setw(14)<<left<<D[ch]->Train_number<<setw(14)<<left<<D[ch]->coach_number;
                cout<<setw(Name_length)<<left<<D[ch]->Name<<setw(14)<<left<<D[ch]->Phone_number<<setw(12)<<left<<D[ch]->date;
                cout<<setw(Complaint_length)<<left<<D[ch]->complaint<<setw(10)<<left<<D[ch]->status<<"\n";
            }
            D[ch]=D[ch]->next;
        }
    }
    else
    {
        cout<<"\nComplaint not registered\n";
    }
}
void Complaint::Convert_list(int id,string pnr,string Train_no,string Coach_no,string Name,string Phone_no,string date,string complaint,string status)
{
    Temp=new struct Record;
    Temp->next=NULL;
    Temp->id=id;
    Temp->pnr=pnr;
    Temp->Train_number=Train_no;
    Temp->coach_number=Coach_no;
    Temp->Name=Name;
    Temp->Phone_number=Phone_no;
    Temp->date=date;
    Temp->complaint=complaint;
    Temp->status=status;
    if(Head==NULL)
    {
        Head=Temp;
        Tail=Head;
        return;
    }
    R=Head;
    while(R->next!=NULL)
    {
        R=R->next;
    }
    R->next=Temp;
    Temp->prev=R;
    Tail=Temp;
}
void Complaint::Login()
{
    cout<<setw(50)<<right<<"\n\nLog in:\n";
    cout<<"\nEnter Mobile: ";
    cin>>Mobile;
    Get_Data();

    if(Mobile.length()==10)
    {
        cout<<"\nName: ";
        getline(cin>>ws,Name);
        if(Name=="")
            getline(cin>>ws,Name);
    }
    if_registered=false;
    R=Head;
    while(R)
    {
        if((R->Name==Name)&&(R->Phone_number==Mobile))
        {
            if_registered=true;
        }
        R=R->next;
    }
    if((Mobile=="9486100250" && Name=="Hari Prasath") ||(Mobile=="7904292862" && Name=="Martin"))
    {
        if_administrator=true;
        if_registered=true;
    }
    else if(if_registered)
    {
        if_administrator=false;
    }
    else
        cout<<"\nLog in ID invalid\n";
    if(if_registered)
        log_out=false;
}
void Complaint::Logout()
{
    log_out=true;
}
void Complaint::Register_Account()
{
    cout<<"Enter User Name: ";
    getline(cin,Name);
    if(Name=="")
        getline(cin,Name);
    cout<<"Mobile number: ";
    getline(cin,Mobile);
    if(Mobile=="")
        getline(cin,Mobile);
    if(Mobile.length()==10)
    {
        Get_complaint();///The user account has to be registered not only after registering complaint
    }
}
void Complaint::Update()
{
    string name,pnr;
    int id;
    int status;///1-completed, 2-pending
    cout<<"\nUpdate:\n1.Close complaint\n2.Undo close\n";
    cin>>status;

    cout<<"PNR: ";
    getline(cin,pnr);
    if(pnr=="")
        getline(cin,pnr);

    string query;
    int qstate;
    R=Head;
    bool is_found=false;
    while(R!=NULL)
    {
        if(if_administrator&&(R->pnr==pnr))
        {
            if(status==1 && R->status=="Pending")
            {
                is_found=true;
                query="UPDATE `complaint` SET status='Completed' WHERE pnr_code='"+pnr+"'";
                R->status="Completed";
            }
            else if(status==2 && R->status=="Completed")
            {
                is_found=true;
                query = "UPDATE `complaint` SET status='Pending' WHERE pnr_code='"+pnr+"'";
                R->status="Pending";
            }
        }
        R=R->next;
    }
    Get_Data();
    const char* q = query.c_str();

    qstate = mysql_query(conn, q);

    if(is_found)
        cout<<"Complaint updated"<<endl;
    else
        cout<<"Doesn't exist"<<endl;
    if(qstate)
        cout<<"query problem: "<<mysql_error(conn)<<endl;
}
void Complaint::Activity()
{
    R=Head;
    int l_n,l_c;
    Name_length=6,Complaint_length=11;
    Empty=true;
    while(R!=NULL)
    {
        if(R->status=="Completed" && (if_administrator))
        {
            l_n=R->Name.length()+2;
            if(Name_length<l_n)
                Name_length=l_n;
            l_c=R->complaint.length()+2;
            if(Complaint_length<l_c)
                Complaint_length=l_c;
            Empty=false;
        }
        R=R->next;
    }
    R=Head;
    if(Empty==false)
    {
        Display_heading();
        while(R!=NULL)
        {
            if(R->status=="Completed" && (if_administrator))
            {
                cout<<setw(12)<<left<<R->pnr<<setw(14)<<left<<R->Train_number<<setw(14)<<left<<R->coach_number;
                cout<<setw(Name_length)<<left<<R->Name<<setw(14)<<left<<R->Phone_number<<setw(12)<<left<<R->date;
                cout<<setw(Complaint_length)<<left<<R->complaint<<setw(10)<<left<<R->status<<"\n";
            }
            R=R->next;
        }
    }
    else
    {
        cout<<"\nNot yet closed any complaints\n";
    }
}
void Complaint::Display(string Name,string Phone)
{
    R=Head;
    int l_n,l_c;
    Name_length=6,Complaint_length=11;
    Empty=true;
    while(R!=NULL)
    {
        if((R->status=="Pending" && (R->Name==Name && R->Phone_number==Phone)) || (if_administrator))
        {
            l_n=R->Name.length()+2;
            if(Name_length<l_n)
                Name_length=l_n;
            l_c=R->complaint.length()+2;
            if(Complaint_length<l_c)
                Complaint_length=l_c;
            Empty=false;
        }
        R=R->next;
    }
    R=Head;
    if(Empty==false)
    {
        Display_heading();
        while(R!=NULL)
        {
            if(((R->status=="Pending" && R->Name==Name && R->Phone_number==Phone && closed_complaints==false)||(closed_complaints&&R->status=="Completed"&&R->Name==Name&&if_administrator==false)) || (if_administrator))
            {
                cout<<setw(12)<<left<<R->pnr<<setw(14)<<left<<R->Train_number<<setw(14)<<left<<R->coach_number;
                cout<<setw(Name_length)<<left<<R->Name<<setw(14)<<left<<R->Phone_number<<setw(12)<<left<<R->date;
                cout<<setw(Complaint_length)<<left<<R->complaint<<setw(10)<<left<<R->status<<"\n";
            }
            R=R->next;
        }
    }
    else
    {
        cout<<"\nComplaint not registered\n";
    }
}
void Complaint::Display_heading()
{
    cout<<setw(12)<<left<<"PNR"<<setw(14)<<left<<"Train Number"<<setw(14)<<left<<"Coach Number";
    cout<<setw(Name_length)<<left<<"Name"<<setw(14)<<left<<"Phone Number"<<setw(12)<<left<<"Date";
    cout<<setw(Complaint_length)<<left<<"Complaint"<<setw(10)<<left<<"Status"<<endl;
}

int Complaint::Connect_sql()
{
    conn = mysql_init(0);
    if(conn)
    {
        //cout << "connection object OK, conn=" << conn << endl;
        conn = mysql_real_connect(conn, "localhost", "phpmyadmin", "hp@0605", "railway_complaint", 0, NULL, 0);
        return 1;
    }
    else
    {
        cout<<"conn object problem: " << mysql_error(conn);
        return 0;
    }
}
void Complaint::Display_Profile()
{
    cout<<endl<<setw(40)<<right<<"User Profile\n";
    cout<<"Name: "<<Name;
    cout<<"\nMobile: "<<Mobile<<"\n";
}
void Complaint::Get_choice()
{
    int ch;
    if(if_administrator && log_out==false)
        cout<<"\n1.Display\n2.Display based on category\n3.Update\n4.Activity\n5.Profile\n6.Log out\n7.Exit\n";
    else if(if_administrator==false && log_out==false)
        cout<<"\n1.Book complaint\n2.History\n3.Based on category\n4.Closed Complaints\n5.Profile\n6.Log out\n7.Exit\n";
    else if(log_out)
        cout<<"\n1.Login\n2.Sign up\n3.Exit\n";
    cin>>ch;
    Execute(ch);
}
void Complaint::Execute(int ch)
{
    closed_complaints=false;
    switch(ch)
    {
    case 1:///Completed
        if(if_administrator==false && log_out==false)
            Get_complaint();
        else if(if_administrator && log_out==false)
        {
            Get_Data();
            Display(Name,Mobile);
        }
        else if(log_out)
            Login();
        Get_choice();
        break;
    case 2:///Completed
        if(log_out==false && Display_Pointer<Display_Limit)
        {
            Get_Data();
        }
        if(if_administrator  && log_out==false)
        {
            int ch;
            cout<<"\n1.Based on Department\n2.Based on Train number\n";
            cin>>ch;
            Get_Data();
            if(ch==1)
                Sort_Department();
            else if(ch==2)
                Sort_Train();
        }
        else if(if_administrator==false  && log_out==false)
        {
            Get_Data();
            cout<<"\nComplaint History\n";
            Display(Name,Mobile);
        }
        else if(log_out)
        {
            cout<<"Sign up\n";
            Register_Account();
        }
        Get_choice();
        break;
    case 3:
        if(if_administrator && log_out==false)
        {
            Get_Data();
            Update();
            Get_choice();
        }
        else if(if_administrator==false && log_out==false)
        {
            Get_Data();
            Sort_Department();
            Get_choice();
        }
        else
            break;
        break;
    case 4:
        if(if_administrator && log_out==false)
        {
            Activity();
            Get_choice();
        }
        else if(if_administrator==false && log_out==false)
        {
            closed_complaints=true;
            Display(Name,Mobile);
            Get_choice();
        }
        else if(log_out)
        {
            Get_choice();
            break;
        }
        break;
    case 5:
        if(if_administrator && log_out==false)
        {
            Display_Profile();
            Get_choice();
        }
        else if(if_administrator==false && log_out==false)
        {
            Display_Profile();
            Get_choice();
        }
        else if(log_out)
            Get_choice();
        break;
    case 6:
        if(if_administrator && log_out==false)
        {
            Logout();
            Get_choice();
        }
        else if(if_administrator==false && log_out==false)
        {
            Logout();
            Get_choice();
        }
        else if(log_out)
            Get_choice();
        break;
    case 7:
        if(if_administrator && log_out==false)
            break;
        else if(if_administrator==false && log_out==false)
            break;
    default:
        Get_choice();
        break;
    }
}
