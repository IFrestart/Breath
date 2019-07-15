#include <iostream>
#include "tinyxml.h"
#include "tinystr.h"
#include <string>
#include <cstring>
using namespace std;

bool ReadXmlFile(string FileName)
{
    TiXmlDocument doc(FileName);
    bool loadOkay = doc.LoadFile();
    // faile to load FileName.
    if ( !loadOkay )
    {
        printf( "Could not load test file %s. Error='%s'. Exiting.\n", filepath,doc.ErrorDesc() );
        exit( 1 );
    }

    // get dom root of FileName, here root should be 'phonebook'.
    TiXmlElement* root = doc.RootElement();

    // trace every items below root.
    for( TiXmlNode* item = root->FirstChild ("EDUDBSvr"); item; item = item->NextSibling ("EDUDBSvr"))
    {
        // read name.
        TiXmlNode* child = item->FirstChild();
        const char* count = child->ToElement()->GetText();
        if (count)
        {
            printf("count:%s\n",count);
        }

        // read address.
        child = item->IterateChildren(child);
        const char* ipaddress0 = child->ToElement()->GetText();
        if (ipaddress0)
        {
            printf("ipaddress0:%s\n",ipaddress0);
        }

        // read telephone no.
        child = item->IterateChildren(child);
        const char* port0 = child->ToElement()->GetText();
        if (port0)
        {
            printf("port0:%s\n",port0);
        }

        // read e-mail.
        child = item->IterateChildren(child);
        const char* user0 = child->ToElement()->GetText();
        if(user0)
        {
            printf("user0:%s\n",user0);
        }

        // read e-mail.
        child = item->IterateChildren(child);
        const char* password0 = child->ToElement()->GetText();
        if(password0)
        {
            printf("password:%s\n",password0);
        }

        // read e-mail.
        child = item->IterateChildren(child);
        const char* dbname0 = child->ToElement()->GetText();
        if(dbname0)
        {
            printf("dbname0:%s\n",dbname0);
        }

        printf("\n");
    }
}

int main()
{
    string fileName = "CenterDBCommServ.xml";
    ReadXmlFile(fileName);
}
