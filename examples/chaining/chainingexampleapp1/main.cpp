#include <MApplication>
#include <MApplicationPage>
#include <MApplicationWindow>
#include <MButton>
#include <MComponentCache>
#include <MExport>

#include "imageviewer.h"

M_EXPORT int main(int argc, char** argv)
{
    MApplication& app = *MComponentCache::mApplication(argc, argv);
    MApplicationWindow& w = *MComponentCache::mApplicationWindow();
    w.setObjectName( "Main Window" );
    w.show();

    MApplicationPage p;
    p.appear(&w);

    MButton b(p.centralWidget());
    b.setText("IMAGE");

    ImageViewer myImageViewer( b );

    return app.exec();
}
