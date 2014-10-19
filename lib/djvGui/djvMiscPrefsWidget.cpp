//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvMiscPrefsWidget.cpp

#include <djvMiscPrefsWidget.h>

#include <djvApplication.h>
#include <djvIntEdit.h>
#include <djvIntObject.h>
#include <djvMiscPrefs.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSequence.h>
#include <djvSignalBlocker.h>

#include <QDialogButtonBox>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvMiscPrefsWidget::P
//------------------------------------------------------------------------------

struct djvMiscPrefsWidget::P
{
    P() :
        timeUnitsWidget        (0),
        speedDefaultWidget     (0),
        sequenceMaxFramesWidget(0),
        toolTipsWidget         (0)
    {}

    QComboBox *  timeUnitsWidget;
    QComboBox *  speedDefaultWidget;
    djvIntEdit * sequenceMaxFramesWidget;
    QCheckBox *  toolTipsWidget;
};

//------------------------------------------------------------------------------
// djvMiscPrefsWidget
//------------------------------------------------------------------------------

djvMiscPrefsWidget::djvMiscPrefsWidget(QWidget * parent) :
    djvAbstractPrefsWidget("Miscellaneous", parent),
    _p(new P)
{
    // Create the time widgets.

    _p->timeUnitsWidget = new QComboBox;
    _p->timeUnitsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->timeUnitsWidget->addItems(djvTime::unitsLabels());

    _p->speedDefaultWidget = new QComboBox;
    _p->speedDefaultWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->speedDefaultWidget->addItems(djvSpeed::fpsLabels());
    
    // Create the sequence widgets.

    _p->sequenceMaxFramesWidget = new djvIntEdit;
    _p->sequenceMaxFramesWidget->setMax(djvIntObject::intMax);
    _p->sequenceMaxFramesWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create the help widgets.

    _p->toolTipsWidget = new QCheckBox("Enable tool tips");

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Time",
        "Set general time options.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow("Time units:", _p->timeUnitsWidget);
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addWidget(_p->speedDefaultWidget);
    hLayout->addWidget(new QLabel("(frames per second)"));
    formLayout->addRow("Default speed:", hLayout);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Sequences",
        "Set general sequence options.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow("Maximum number of frames:", _p->sequenceMaxFramesWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Help",
        "Set general help options.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->toolTipsWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->timeUnitsWidget,
        SIGNAL(activated(int)),
        SLOT(timeUnitsCallback(int)));

    connect(
        _p->speedDefaultWidget,
        SIGNAL(activated(int)),
        SLOT(speedDefaultCallback(int)));

    connect(
        _p->sequenceMaxFramesWidget,
        SIGNAL(valueChanged(int)),
        SLOT(sequenceMaxFramesCallback(int)));

    connect(
        _p->toolTipsWidget,
        SIGNAL(toggled(bool)),
        SLOT(helpToolTipsCallback(bool)));
}

djvMiscPrefsWidget::~djvMiscPrefsWidget()
{
    delete _p;
}

void djvMiscPrefsWidget::resetPreferences()
{
    djvTime::resetUnits();
    
    djvMiscPrefs::global()->setTimeUnits(djvTime::units());
    
    djvSpeed::resetSpeedDefault();

    djvMiscPrefs::global()->setSpeedDefault(djvSpeed::speedDefault());

    djvSequence::resetMaxFrames();

    djvMiscPrefs::global()->setSequenceMaxFrames(djvSequence::maxFrames());

    DJV_APP->setToolTips(djvApplication::toolTipsDefault());
    
    widgetUpdate();
}

void djvMiscPrefsWidget::timeUnitsCallback(int index)
{
    djvMiscPrefs::global()->setTimeUnits(static_cast<djvTime::UNITS>(index));
}

void djvMiscPrefsWidget::speedDefaultCallback(int index)
{
    djvMiscPrefs::global()->setSpeedDefault(static_cast<djvSpeed::FPS>(index));
}

void djvMiscPrefsWidget::sequenceMaxFramesCallback(int size)
{
    djvMiscPrefs::global()->setSequenceMaxFrames(size);
}

void djvMiscPrefsWidget::helpToolTipsCallback(bool toolTips)
{
    DJV_APP->setToolTips(toolTips);
}

void djvMiscPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->timeUnitsWidget <<
        _p->speedDefaultWidget <<
        _p->sequenceMaxFramesWidget <<
        _p->toolTipsWidget);

    _p->timeUnitsWidget->setCurrentIndex(djvTime::units());
    
    _p->speedDefaultWidget->setCurrentIndex(djvSpeed::speedDefault());

    _p->sequenceMaxFramesWidget->setValue(djvSequence::maxFrames());

    _p->toolTipsWidget->setChecked(DJV_APP->hasToolTips());
}
