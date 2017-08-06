#include "ccseparatethread.h"
#include "libfusecc.h"

QCoreApplication* CCSeparateThread::a=NULL;
int CCSeparateThread::argc = 1;
char * CCSeparateThread::argv[] = {"SharedLibrary", NULL};

CCSeparateThread::CCSeparateThread(QObject *parent) : QObject(parent){

}





bool CCSeparateThread::command_getRemoteFileList(){

    MSCloudProvider* cp = lpProviderObject;
     lpFuseCC->readRemoteFileList(cp);
     //qDebug() << QString::number((qlonglong)cp)<< " THR";
    return true;

}

bool CCSeparateThread::command_getFileContent(){

    QString cachePath = commandParameters["cachePath"].toString(); //this->params["params"].toObject()["cachePath"].toString();
    QString filePath = commandParameters["filePath"].toString(); //this->params["params"].toObject()["filePath"].toString();
    QString pathToCache= cachePath.replace(filePath,""); //cachePath.replace(filePath,"");


    log("getFileContent "+filePath+"   "+ pathToCache);

    QHash<QString,MSFSObject>::iterator i = lpProviderObject->syncFileList.find(filePath);

    if(i != lpProviderObject->syncFileList.end()){
        MSFSObject obj = i.value();

        MSCloudProvider* cp = lpProviderObject;

        lpFuseCC->readFileContent(cp,pathToCache,obj);


//        QString b =cp->workPath;
//        cp->workPath = pathToCache;
//        cp->remote_file_get(&obj);
//        cp->workPath = b;

        log("getFileContent CloudProvider remote_file_get executed");
        return true;
    }
     //this->workerPtr->fileList.find(filePath).value();



    log("getFileContent ERROR!!!!");
    return  false;
}



bool CCSeparateThread::command_doSync(){

    QString cachePath = commandParameters["cachePath"].toString();

    MSCloudProvider* cp = lpProviderObject;

    QHash<QString,MSFSObject> back = cp->syncFileList;
    QHash<QString,MSFSObject> work;// = cp->syncFileList;


    QHash<QString,MSFSObject>::iterator i = cp->syncFileList.begin();

    for(i ;i != cp->syncFileList.end();i++){

        if( ((i.value().state == MSFSObject::ObjectState::ChangedLocal) ||
            (i.value().state == MSFSObject::ObjectState::NewLocal) ||
            (i.value().state == MSFSObject::ObjectState::DeleteLocal)) &&
            (i.key().indexOf(cachePath) != 0)){

            work.insert(i.key(),i.value());
        }

    }

    cp->setFlag("dryRun",true);
    cp->dryRun = true;
    cp->syncFileList = work;
    cp->doSync();

     //lpFuseCC->readRemoteFileList(cp);
     //qDebug() << QString::number((qlonglong)cp)<< " THR";
    return true;


}



void CCSeparateThread::run(){

    if(this->commandToExecute == "get_remote_file_list"){

        this->command_getRemoteFileList();

    }

    if(this->commandToExecute == "get_content"){

        this->command_getFileContent();

    }


    if(this->commandToExecute == "sync"){

        this->command_doSync();

    }

    emit finished();
    return ;

}



void CCSeparateThread::log(QString mes){

    FILE* lf=fopen("/tmp/ccfw.log","a+");
    if(lf != NULL){

        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer,sizeof(buffer),"%T - ",timeinfo);

        mes = QString(buffer)+QString("SEP_THR ")+mes+" \n";
        fputs(mes.toStdString().c_str(),lf);
        fclose(lf);
    }

//    string ns="echo "+mes+" >> /tmp/ccfw.log ";
//    system(ns.c_str());
    return;
}


