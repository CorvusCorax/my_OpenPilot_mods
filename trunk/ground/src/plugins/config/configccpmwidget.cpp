/**
 ******************************************************************************
 *
 * @file       configccpmwidget.cpp
 * @author     E. Lafargue & The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup ConfigPlugin Config Plugin
 * @{
 * @brief ccpm configuration panel
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include "configccpmwidget.h"

#include <QDebug>
#include <QStringList>
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <math.h>

#define  Pi 3.14159265358979323846


ConfigccpmWidget::ConfigccpmWidget(QWidget *parent) : ConfigTaskWidget(parent)
{
    m_ccpm = new Ui_ccpmWidget();
    m_ccpm->setupUi(this);

    // Now connect the widget to the ManualControlCommand / Channel UAVObject
    //ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    //UAVObjectManager *objManager = pm->getObject<UAVObjectManager>();

    // Initialization of the swashplaye widget
    m_ccpm->SwashplateImage->setScene(new QGraphicsScene(this));

    QSvgRenderer *renderer = new QSvgRenderer();
    renderer->load(QString(":/configgadget/images/ccpm_setup.svg"));


    SwashplateImg = new QGraphicsSvgItem();
    SwashplateImg->setSharedRenderer(renderer);
    SwashplateImg->setElementId("Swashplate");
    SwashplateImg->setObjectName("Swashplate");
    m_ccpm->SwashplateImage->scene()->addItem(SwashplateImg);
    m_ccpm->SwashplateImage->setSceneRect(SwashplateImg->boundingRect());

    m_ccpm->SwashplateImage->scale(.75,.75);

    ServoW = new QGraphicsSvgItem();
    ServoW->setSharedRenderer(renderer);
    ServoW->setElementId("ServoW");
    m_ccpm->SwashplateImage->scene()->addItem(ServoW);

    ServoX = new QGraphicsSvgItem();
    ServoX->setSharedRenderer(renderer);
    ServoX->setElementId("ServoX");
    m_ccpm->SwashplateImage->scene()->addItem(ServoX);

    ServoY = new QGraphicsSvgItem();
    ServoY->setSharedRenderer(renderer);
    ServoY->setElementId("ServoY");
    m_ccpm->SwashplateImage->scene()->addItem(ServoY);

    ServoZ = new QGraphicsSvgItem();
    ServoZ->setSharedRenderer(renderer);
    ServoZ->setElementId("ServoZ");
    m_ccpm->SwashplateImage->scene()->addItem(ServoZ);

    QFont serifFont("Times", 16, QFont::Bold);

    ServoWText = new QGraphicsTextItem();
    ServoWText->setDefaultTextColor(Qt::red);
    ServoWText->setPlainText(QString("-"));
    ServoWText->setFont(serifFont);
    m_ccpm->SwashplateImage->scene()->addItem(ServoWText);

    ServoXText = new QGraphicsTextItem();
    ServoXText->setDefaultTextColor(Qt::red);
    ServoXText->setPlainText(QString("-"));
    ServoXText->setFont(serifFont);
    m_ccpm->SwashplateImage->scene()->addItem(ServoXText);

    ServoYText = new QGraphicsTextItem();
    ServoYText->setDefaultTextColor(Qt::red);
    ServoYText->setPlainText(QString("-"));
    ServoYText->setFont(serifFont);
    m_ccpm->SwashplateImage->scene()->addItem(ServoYText);

    ServoZText = new QGraphicsTextItem();
    ServoZText->setDefaultTextColor(Qt::red);
    ServoZText->setPlainText(QString("-"));
    ServoZText->setFont(serifFont);
    m_ccpm->SwashplateImage->scene()->addItem(ServoZText);





    QStringList channels;
    channels << "Channel0" << "Channel1" << "Channel2" <<
            "Channel3" << "Channel4" << "Channel5" << "Channel6" << "Channel7" << "None" ;
    m_ccpm->ccpmEngineChannel->addItems(channels);
    m_ccpm->ccpmEngineChannel->setCurrentIndex(8);
    m_ccpm->ccpmTailChannel->addItems(channels);
    m_ccpm->ccpmTailChannel->setCurrentIndex(8);
    m_ccpm->ccpmServoWChannel->addItems(channels);
    m_ccpm->ccpmServoWChannel->setCurrentIndex(8);
    m_ccpm->ccpmServoXChannel->addItems(channels);
    m_ccpm->ccpmServoXChannel->setCurrentIndex(8);
    m_ccpm->ccpmServoYChannel->addItems(channels);
    m_ccpm->ccpmServoYChannel->setCurrentIndex(8);
    m_ccpm->ccpmServoZChannel->addItems(channels);
    m_ccpm->ccpmServoZChannel->setCurrentIndex(8);

    QStringList Types;
    Types << "CCPM 2 Servo 90�" << "CCPM 3 Servo 120�" << "CCPM 3 Servo 140�" << "Custom - User Angles" << "Custom - Advanced Settings"  ;
    m_ccpm->ccpmType->addItems(Types);
    m_ccpm->ccpmType->setCurrentIndex(m_ccpm->ccpmType->count() - 1);
    requestccpmUpdate();
    UpdateCurveSettings();

    //disable changing number of points in curves until UAVObjects have more than 5
    m_ccpm->NumCurvePoints->setEnabled(0);

    UpdateType();

    connect(m_ccpm->saveccpmToSD, SIGNAL(clicked()), this, SLOT(saveccpmUpdate()));
    connect(m_ccpm->saveccpmToRAM, SIGNAL(clicked()), this, SLOT(sendccpmUpdate()));
    connect(m_ccpm->getccpmCurrent, SIGNAL(clicked()), this, SLOT(requestccpmUpdate()));
    connect(m_ccpm->ccpmGenerateCurve, SIGNAL(clicked()), this, SLOT(GenerateCurve()));
    connect(m_ccpm->NumCurvePoints, SIGNAL(valueChanged(int)), this, SLOT(UpdateCurveSettings()));
    connect(m_ccpm->CurveType, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateCurveSettings()));
    connect(m_ccpm->ccpmAngleW, SIGNAL(valueChanged(double)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmAngleX, SIGNAL(valueChanged(double)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmAngleY, SIGNAL(valueChanged(double)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmAngleZ, SIGNAL(valueChanged(double)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmCorrectionAngle, SIGNAL(valueChanged(double)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmServoWChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmServoXChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmServoYChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmServoZChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(ccpmSwashplateUpdate()));
    connect(m_ccpm->ccpmEngineChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateMixer()));
    connect(m_ccpm->ccpmTailChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateMixer()));
    connect(m_ccpm->ccpmRevoSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateMixer()));
    connect(m_ccpm->ccpmREVOspinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateMixer()));
    connect(m_ccpm->ccpmCollectiveSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateMixer()));
    connect(m_ccpm->ccpmCollectivespinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateMixer()));
    connect(m_ccpm->ccpmType, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateType()));
    connect(m_ccpm->ccpmSingleServo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateType()));





    connect(parent, SIGNAL(autopilotConnected()),this, SLOT(requestccpmUpdate()));

}

ConfigccpmWidget::~ConfigccpmWidget()
{
   // Do nothing
}

void ConfigccpmWidget::UpdateType()
{
    int TypeInt,SingleServoIndex;
    QString TypeText;


    TypeInt = m_ccpm->ccpmType->count() - m_ccpm->ccpmType->currentIndex()-1;
    TypeText = m_ccpm->ccpmType->currentText();
    SingleServoIndex = m_ccpm->ccpmSingleServo->currentIndex();

    //set visibility of user settings
    m_ccpm->ccpmAdvancedSettingsTable->setEnabled(TypeInt==0);
    m_ccpm->ccpmAdvancedSettingsTable->clearFocus();;

    m_ccpm->ccpmAngleW->setEnabled(TypeInt==1);
    m_ccpm->ccpmAngleX->setEnabled(TypeInt==1);
    m_ccpm->ccpmAngleY->setEnabled(TypeInt==1);
    m_ccpm->ccpmAngleZ->setEnabled(TypeInt==1);
    m_ccpm->ccpmCorrectionAngle->setEnabled(TypeInt==1);

    m_ccpm->ccpmServoWChannel->setEnabled(TypeInt>0);
    m_ccpm->ccpmServoXChannel->setEnabled(TypeInt>0);
    m_ccpm->ccpmServoYChannel->setEnabled(TypeInt>0);
    m_ccpm->ccpmServoZChannel->setEnabled(TypeInt>0);
    m_ccpm->ccpmSingleServo->setEnabled(TypeInt>1);

    m_ccpm->ccpmEngineChannel->setEnabled(TypeInt>0);
    m_ccpm->ccpmTailChannel->setEnabled(TypeInt>0);
    m_ccpm->ccpmCollectiveSlider->setEnabled(TypeInt>0);
    m_ccpm->ccpmCollectivespinBox->setEnabled(TypeInt>0);
    m_ccpm->ccpmRevoSlider->setEnabled(TypeInt>0);
    m_ccpm->ccpmREVOspinBox->setEnabled(TypeInt>0);


    //set values for pre defined heli types
        if (TypeText.compare(QString("CCPM 2 Servo 90�"), Qt::CaseInsensitive)==0)
        {
            m_ccpm->ccpmAngleW->setValue(0);
            m_ccpm->ccpmAngleX->setValue(90);
            m_ccpm->ccpmAngleY->setValue(0);
            m_ccpm->ccpmAngleZ->setValue(0);
            m_ccpm->ccpmAngleY->setEnabled(0);
            m_ccpm->ccpmAngleZ->setEnabled(0);
            m_ccpm->ccpmServoYChannel->setEnabled(0);
            m_ccpm->ccpmServoZChannel->setEnabled(0);
            m_ccpm->ccpmCorrectionAngle->setValue(SingleServoIndex*90);
        }
        if (TypeText.compare(QString("CCPM 3 Servo 120�"), Qt::CaseInsensitive)==0)
        {
            m_ccpm->ccpmAngleW->setValue(0);
            m_ccpm->ccpmAngleX->setValue(120);
            m_ccpm->ccpmAngleY->setValue(240);
            m_ccpm->ccpmAngleZ->setValue(0);
            m_ccpm->ccpmAngleZ->setEnabled(0);
            m_ccpm->ccpmServoZChannel->setEnabled(0);
            m_ccpm->ccpmCorrectionAngle->setValue(SingleServoIndex*90);
        }
        if (TypeText.compare(QString("CCPM 3 Servo 140�"), Qt::CaseInsensitive)==0)
        {
            m_ccpm->ccpmAngleW->setValue(0);
            m_ccpm->ccpmAngleX->setValue(140);
            m_ccpm->ccpmAngleY->setValue(220);
            m_ccpm->ccpmAngleZ->setValue(0);
            m_ccpm->ccpmAngleZ->setEnabled(0);
            m_ccpm->ccpmServoZChannel->setEnabled(0);
            m_ccpm->ccpmCorrectionAngle->setValue(SingleServoIndex*90);
        }


    //update UI
    ccpmSwashplateUpdate();

}




void ConfigccpmWidget::UpdateCurveSettings()
{
    int NumCurvePoints,i;
    double scale;
    QString CurveType;
    QStringList vertHeaders;

    //get the user settings
    NumCurvePoints=m_ccpm->NumCurvePoints->value();
    CurveType=m_ccpm->CurveType->currentText();

    vertHeaders << "-" << "-" << "-" << "-" << "-" << "-" << "-" << "-" << "-" << "-" ;
    for (i=0;i<NumCurvePoints;i++)
    {
        scale =((double)i/(double)(NumCurvePoints-1));
        vertHeaders[i] = tr( "%1%" ).arg(100.00*scale, 0, 'f', 1);
    }
    m_ccpm->CurveSettings->setVerticalHeaderLabels( vertHeaders );

    if ( CurveType.compare("Flat")==0)
    {
        m_ccpm->CurveLabel1->setText("Value");
        m_ccpm->CurveLabel1->setVisible(true);
        m_ccpm->CurveValue1->setVisible(true);
        m_ccpm->CurveLabel2->setVisible(false);
        m_ccpm->CurveValue2->setVisible(false);
        m_ccpm->CurveLabel3->setVisible(false);
        m_ccpm->CurveValue3->setVisible(false);
        m_ccpm->ccpmGenerateCurve->setVisible(true);
        m_ccpm->CurveToGenerate->setVisible(true);
    }
    if ( CurveType.compare("Linear")==0)
    {
        m_ccpm->CurveLabel1->setText("Min");
        m_ccpm->CurveLabel1->setVisible(true);
        m_ccpm->CurveValue1->setVisible(true);
        m_ccpm->CurveLabel2->setText("Max");
        m_ccpm->CurveLabel2->setVisible(true);
        m_ccpm->CurveValue2->setVisible(true);
        m_ccpm->CurveLabel3->setVisible(false);
        m_ccpm->CurveValue3->setVisible(false);
        m_ccpm->ccpmGenerateCurve->setVisible(true);
        m_ccpm->CurveToGenerate->setVisible(true);
    }
    if ( CurveType.compare("Step")==0)
    {
        m_ccpm->CurveLabel1->setText("Min");
        m_ccpm->CurveLabel1->setVisible(true);
        m_ccpm->CurveValue1->setVisible(true);
        m_ccpm->CurveLabel2->setText("Max");
        m_ccpm->CurveLabel2->setVisible(true);
        m_ccpm->CurveValue2->setVisible(true);
        m_ccpm->CurveLabel3->setText("Step at");
        m_ccpm->CurveLabel3->setVisible(true);
        m_ccpm->CurveValue3->setVisible(true);
        m_ccpm->ccpmGenerateCurve->setVisible(true);
        m_ccpm->CurveToGenerate->setVisible(true);
    }
    if ( CurveType.compare("Custom")==0)
    {
        m_ccpm->CurveLabel1->setVisible(false);
        m_ccpm->CurveValue1->setVisible(false);
        m_ccpm->CurveLabel2->setVisible(false);
        m_ccpm->CurveValue2->setVisible(false);
        m_ccpm->CurveLabel3->setVisible(false);
        m_ccpm->CurveValue3->setVisible(false);
        m_ccpm->ccpmGenerateCurve->setVisible(false);
        m_ccpm->CurveToGenerate->setVisible(false);
    }

}
void ConfigccpmWidget::GenerateCurve()
{
   int NumCurvePoints,CurveToGenerate,i;
   double value1, value2, value3, scale;
   QString CurveType;
   QTableWidgetItem *item;


   //get the user settings
   NumCurvePoints=m_ccpm->NumCurvePoints->value();
   value1=m_ccpm->CurveValue1->value();
   value2=m_ccpm->CurveValue2->value();
   value3=m_ccpm->CurveValue3->value();
   CurveToGenerate=m_ccpm->CurveToGenerate->currentIndex();
   CurveType=m_ccpm->CurveType->currentText();



   for (i=0;i<NumCurvePoints;i++)
   {
       scale =((double)i/(double)(NumCurvePoints-1));
       item =m_ccpm->CurveSettings->item(i, CurveToGenerate );

       if ( CurveType.compare("Flat")==0)
       {
           item->setText( tr( "%1" ).arg( value1 ) );
       }
       if ( CurveType.compare("Linear")==0)
       {
           item->setText( tr( "%1" ).arg(value1 +(scale*(value2-value1))) );
       }
       if ( CurveType.compare("Step")==0)
       {
           if (scale*100<value3)
           {
               item->setText( tr( "%1" ).arg(value1) );
           }
           else
           {
               item->setText( tr( "%1" ).arg(value2) );
           }
       }
   }
   for (i=NumCurvePoints;i<10;i++)
   {
       item =m_ccpm->CurveSettings->item(i, CurveToGenerate );
       item->setText( tr( "" ) );
   }


}

void ConfigccpmWidget::ccpmSwashplateUpdate()
{
    double angle,CorrectionAngle,x,y,CenterX,CenterY;
    int used;

    CorrectionAngle=m_ccpm->ccpmCorrectionAngle->value();

    //CenterX=m_ccpm->SwashplateImage->scene()->sceneRect().center().x();
   // CenterY=m_ccpm->SwashplateImage->scene()->sceneRect().center().y();
    CenterX=200;
    CenterY=220;

    SwashplateImg->setPos(CenterX-200,CenterY-200);

    used=((m_ccpm->ccpmServoWChannel->currentIndex()<8)&&(m_ccpm->ccpmServoWChannel->isEnabled()));
    ServoW->setVisible(used!=0);
    ServoWText->setVisible(used!=0);
    angle=(CorrectionAngle+180+m_ccpm->ccpmAngleW->value())*Pi/180.00;
    x=CenterX-(200.00*sin(angle))-10.00;
    y=CenterY+(200.00*cos(angle))-10.00;
    ServoW->setPos(x, y);
    x=CenterX-(170.00*sin(angle))-10.00;
    y=CenterY+(170.00*cos(angle))-10.00;
    ServoWText->setPos(x, y);

    used=((m_ccpm->ccpmServoXChannel->currentIndex()<8)&&(m_ccpm->ccpmServoXChannel->isEnabled()));
    ServoX->setVisible(used!=0);
    ServoXText->setVisible(used!=0);
    angle=(CorrectionAngle+180+m_ccpm->ccpmAngleX->value())*Pi/180.00;
    x=CenterX-(200.00*sin(angle))-10.00;
    y=CenterY+(200.00*cos(angle))-10.00;
    ServoX->setPos(x, y);
    x=CenterX-(170.00*sin(angle))-10.00;
    y=CenterY+(170.00*cos(angle))-10.00;
    ServoXText->setPos(x, y);

    used=((m_ccpm->ccpmServoYChannel->currentIndex()<8)&&(m_ccpm->ccpmServoYChannel->isEnabled()));
    ServoY->setVisible(used!=0);
    ServoYText->setVisible(used!=0);
    angle=(CorrectionAngle+180+m_ccpm->ccpmAngleY->value())*Pi/180.00;
    x=CenterX-(200.00*sin(angle))-10.00;
    y=CenterY+(200.00*cos(angle))-10.00;
    ServoY->setPos(x, y);
    x=CenterX-(170.00*sin(angle))-10.00;
    y=CenterY+(170.00*cos(angle))-10.00;
    ServoYText->setPos(x, y);

    used=((m_ccpm->ccpmServoZChannel->currentIndex()<8)&&(m_ccpm->ccpmServoZChannel->isEnabled()));
    ServoZ->setVisible(used!=0);
    ServoZText->setVisible(used!=0);
    angle=(CorrectionAngle+180+m_ccpm->ccpmAngleZ->value())*Pi/180.00;
    x=CenterX-(200.00*sin(angle))-10.00;
    y=CenterY+(200.00*cos(angle))-10.00;
    ServoZ->setPos(x, y);
    x=CenterX-(170.00*sin(angle))-10.00;
    y=CenterY+(170.00*cos(angle))-10.00;
    ServoZText->setPos(x, y);

    //m_ccpm->SwashplateImage->centerOn (CenterX, CenterY);


    UpdateMixer();

}

void ConfigccpmWidget::UpdateMixer()
{
    int i,j,Type,ThisEnable[6];
    float CollectiveConstant,CorrectionAngle,ThisAngle[6];
    //QTableWidgetItem *newItem;// = new QTableWidgetItem();
    QString Channel;

    Type = m_ccpm->ccpmType->count() - m_ccpm->ccpmType->currentIndex()-1;
    CollectiveConstant=m_ccpm->ccpmCollectiveSlider->value()/100.0;
    CorrectionAngle=m_ccpm->ccpmCorrectionAngle->value();


    if (Type>0)
    {//not advanced settings
        //get the channel data from the ui
        MixerChannelData[0] = m_ccpm->ccpmEngineChannel->currentIndex();
        MixerChannelData[1] = m_ccpm->ccpmTailChannel->currentIndex();
        MixerChannelData[2] = m_ccpm->ccpmServoWChannel->currentIndex();
        MixerChannelData[3] = m_ccpm->ccpmServoXChannel->currentIndex();
        MixerChannelData[4] = m_ccpm->ccpmServoYChannel->currentIndex();
        MixerChannelData[5] = m_ccpm->ccpmServoZChannel->currentIndex();

        //get the angle data from the ui
        ThisAngle[2] = m_ccpm->ccpmAngleW->value();
        ThisAngle[3] = m_ccpm->ccpmAngleX->value();
        ThisAngle[4] = m_ccpm->ccpmAngleY->value();
        ThisAngle[5] = m_ccpm->ccpmAngleZ->value();

        //get the angle data from the ui
        ThisEnable[2] = m_ccpm->ccpmServoWChannel->isEnabled();
        ThisEnable[3] = m_ccpm->ccpmServoXChannel->isEnabled();
        ThisEnable[4] = m_ccpm->ccpmServoYChannel->isEnabled();
        ThisEnable[5] = m_ccpm->ccpmServoZChannel->isEnabled();

        ServoWText->setPlainText(QString("%1").arg( MixerChannelData[2] ));
        ServoXText->setPlainText(QString("%1").arg( MixerChannelData[3] ));
        ServoYText->setPlainText(QString("%1").arg( MixerChannelData[4] ));
        ServoZText->setPlainText(QString("%1").arg( MixerChannelData[5] ));


        //go through the user data and update the mixer matrix
        for (i=0;i<6;i++)
        {
            /*
                data.Mixer0Type = 0;//Disabled,Motor,Servo
                data.Mixer0Vector[0] = 0;//ThrottleCurve1
                data.Mixer0Vector[1] = 0;//ThrottleCurve2
                data.Mixer0Vector[2] = 0;//Roll
                data.Mixer0Vector[3] = 0;//Pitch
                data.Mixer0Vector[4] = 0;//Yaw

            */
            if ((MixerChannelData[i]<8)&&(ThisEnable[i]))
            {
                m_ccpm->ccpmAdvancedSettingsTable->item(i,0)->setText(QString("%1").arg( MixerChannelData[i] ));
                 //config the vector
               if (i==0)
                {//motor-engine
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,1)->setText(QString("%1").arg(127));//ThrottleCurve1
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,2)->setText(QString("%1").arg(0));//ThrottleCurve2
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,3)->setText(QString("%1").arg(0));//Roll
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,4)->setText(QString("%1").arg(0));//Pitch
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,5)->setText(QString("%1").arg(0));//Yaw
                }
                if (i==1)
                {//tailrotor
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,1)->setText(QString("%1").arg(0));//ThrottleCurve1
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,2)->setText(QString("%1").arg(0));//ThrottleCurve2
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,3)->setText(QString("%1").arg(0));//Roll
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,4)->setText(QString("%1").arg(0));//Pitch
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,5)->setText(QString("%1").arg(127));//Yaw
                }
                if (i>1)
                {//Swashplate
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,1)->setText(QString("%1").arg(0));//ThrottleCurve1
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,2)->setText(QString("%1").arg((int)(127.0*CollectiveConstant)));//ThrottleCurve2
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,3)->setText(QString("%1").arg((int)(127.0*(1-CollectiveConstant)*sin((CorrectionAngle + ThisAngle[i])*Pi/180.00))));//Roll
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,4)->setText(QString("%1").arg((int)(127.0*(1-CollectiveConstant)*cos((CorrectionAngle + ThisAngle[i])*Pi/180.00))));//Pitch
                    m_ccpm->ccpmAdvancedSettingsTable->item(i,5)->setText(QString("%1").arg(0));//Yaw

                }
            }
            else
            {
                for (j=0;j<6;j++) m_ccpm->ccpmAdvancedSettingsTable->item(i,j)->setText(QString("-"));
            }

        }
    }
    else
    {//advanced settings
         for (i=0;i<6;i++)
         {
             Channel =m_ccpm->ccpmAdvancedSettingsTable->item(i,0)->text();
             if (Channel == "-") Channel = QString("8");
             MixerChannelData[i]= Channel.toInt();
         }
    }



}

/**************************
  * ccpm settings
  **************************/
/**
  Request the current value of the SystemSettings which holds the ccpm type
  */
void ConfigccpmWidget::requestccpmUpdate()
{
    //ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    //UAVObjectManager *objManager = pm->getObject<UAVObjectManager>();


    //UAVObjectField *field;
    //QTableWidgetItem *newItem;// = new QTableWidgetItem();


	//not doing anything yet


    ccpmSwashplateUpdate();

}


/**
  Sends the config to the board (ccpm type)
  */
void ConfigccpmWidget::sendccpmUpdate()
{
    int i,j;
    UAVObjectField *field;
    UAVDataObject* obj;

    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    UAVObjectManager *objManager = pm->getObject<UAVObjectManager>();

    obj = dynamic_cast<UAVDataObject*>(objManager->getObject(QString("MixerSettings")));
        Q_ASSERT(obj);

        UpdateMixer();

        //go through the user data and update the mixer matrix
        for (i=0;i<6;i++)
        {
            /*
                data.Mixer0Type = 0;//Disabled,Motor,Servo
                data.Mixer0Vector[0] = 0;//ThrottleCurve1
                data.Mixer0Vector[1] = 0;//ThrottleCurve2
                data.Mixer0Vector[2] = 0;//Roll
                data.Mixer0Vector[3] = 0;//Pitch
                data.Mixer0Vector[4] = 0;//Yaw

            */
            if (MixerChannelData[i]<8)
            {
                //select the correct mixer for this config element
                field = obj->getField(tr( "Mixer%1Type" ).arg( MixerChannelData[i] ));
                //set the mixer type
                if (i==0)
                {
                    field->setValue("Motor");
                }
                else
                {
                    field->setValue("Servo");
                }

                //select the correct mixer for this config element
                field = obj->getField(tr( "Mixer%1Vector" ).arg( MixerChannelData[i] ));
                //config the vector
                for (j=0;j<5;j++)
                {
                    //field->setValue(j,m_ccpm->ccpmAdvancedSettingsTable->item(i,j+1)->text().toInt());
                }

            }

        }


 		//get the user data for the curve into the mixer settings
       field = obj->getField(QString("ThrottleCurve1"));
        for (i=0;i<5;i++)
        {
            field->setValue(m_ccpm->CurveSettings->item(i, 0)->text().toDouble(),i);
        }
        field = obj->getField(QString("ThrottleCurve2"));
        for (i=0;i<5;i++)
        {
            field->setValue(m_ccpm->CurveSettings->item(i, 1)->text().toDouble(),i);
        }

        obj->updated();


}

/**
  Send ccpm type to the board and request saving to SD card
  */
void ConfigccpmWidget::saveccpmUpdate()
{
    // Send update so that the latest value is saved
    sendccpmUpdate();
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    UAVObjectManager *objManager = pm->getObject<UAVObjectManager>();
    UAVDataObject* obj = dynamic_cast<UAVDataObject*>(objManager->getObject(QString("MixerSettings")));
    Q_ASSERT(obj);
    updateObjectPersistance(ObjectPersistence::OPERATION_SAVE, obj);
}

