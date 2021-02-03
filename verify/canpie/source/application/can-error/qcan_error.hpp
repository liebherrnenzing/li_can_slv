//============================================================================//
// File:          qcan_error.hpp                                              //
// Description:   Send CAN error messages                                     //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'LICENSE'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
//============================================================================//

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

#include <QCanSocket>

class QCanError : public QObject
{
   Q_OBJECT

public:
   QCanError(QObject *parent = 0);

signals:
   void finished();

public slots:
   void aboutToQuitApp(void);

   void runCmdParser(void);

   void sendErrorFrame(void);
   void socketConnected();
   void socketDisconnected();
   void socketError(QAbstractSocket::SocketError teSocketErrorV);
   void quit();
   
private:

   QCoreApplication *   pclAppP;

   QCommandLineParser   clCmdParserP;
   QCanSocket           clCanSocketP;
   uint8_t              ubChannelP;
   
   //----------------------------------------------------------------
   // values of receive and transmit error counter
   //
   uint8_t              ubRcvErrorCntP;
   uint8_t              ubTrmErrorCntP;

   //----------------------------------------------------------------
   // flags for decrementing the receive and transmit error counter
   //
   bool                 btDecRcvErrorP;
   bool                 btDecTrmErrorP;

   //----------------------------------------------------------------
   // flags for incrementing the receive and transmit error counter
   //
   bool                 btIncRcvErrorP;
   bool                 btIncTrmErrorP;

   QCanFrame::ErrorType_e   teErrorTypeP;

   QCanFrame            clErrorFrameP;
   uint32_t             ulFrameGapP;
   uint32_t             ulFrameCountP;
};



