//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#pragma once

#include <djvOpenGlWidget.h>

#include <djvOpenGlImage.h>

#include <memory>

class djvGuiContext;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvImageView
//!
//! This class provides an image view widget.
//------------------------------------------------------------------------------

class djvImageView : public djvOpenGlWidget
{
    Q_OBJECT
    
    //! This property holds the pixel data.
    Q_PROPERTY(
        const djvPixelData * data
        READ                 data
        WRITE                setData
        NOTIFY               dataChanged)
    
    //! This property holds the image options.
    Q_PROPERTY(
        djvOpenGlImageOptions options
        READ                  options
        WRITE                 setOptions
        NOTIFY                optionsChanged)
    
    //! This property holds the view position.
    Q_PROPERTY(
        glm::ivec2 viewPos
        READ       viewPos
        WRITE      setViewPos
        NOTIFY     viewPosChanged)
    
    //! This property holds the view zoom.
    Q_PROPERTY(
        float  viewZoom
        READ   viewZoom
        WRITE  setViewZoom
        NOTIFY viewZoomChanged)
    
public:
    explicit djvImageView(
        djvGuiContext *   context,
        QWidget *         parent  = nullptr,
        Qt::WindowFlags   flags   = 0);

    virtual ~djvImageView();

    //! Get the pixel data.
    const djvPixelData * data() const;

    //! Get the image options.
    const djvOpenGlImageOptions & options() const;

    //! Get the view position.
    const glm::ivec2 & viewPos() const;

    //! Get the view zoom.
    float viewZoom() const;

    //! Get the image bounding box.
    djvBox2f bbox() const;
    
    //! Get whether the view has been fitted.
    bool hasViewFit() const;

    virtual QSize sizeHint() const;

public Q_SLOTS:
    //! Set the pixel data.
    void setData(const djvPixelData *);

    //! Set the image options.
    void setOptions(const djvOpenGlImageOptions &);

    //! Set the view position.
    void setViewPos(const glm::ivec2 &);

    //! Set the view zoom.
    void setViewZoom(float);

    //! Set the view zoom.
    void setViewZoom(float, const glm::ivec2 & focus);

    //! Set the view position and zoom.
    void setViewPosZoom(const glm::ivec2 &, float zoom);

    //! Reset the view position.
    void viewZero();

    //! Center the view position.
    void viewCenter();

    //! Adjust the zoom to fit the view.
    void viewFit();

Q_SIGNALS:
    //! This signal is emitted when the pixel data is changed.
    void dataChanged(const djvPixelData *);

    //! This signal is emitted when the image options are changed.
    void optionsChanged(const djvOpenGlImageOptions &);

    //! This signal is emitted when the view position is changed.
    void viewPosChanged(const glm::ivec2 &);

    //! This signal is emitted when the view zoom is changed.
    void viewZoomChanged(float);

    //! This signal is emitted when the view is changed.
    void viewChanged();

    //! This signal is emitted when the view is fitted.
    void viewFitted();

protected:
    virtual void paintGL();

private:
    djvBox2f bbox(const glm::ivec2 &, float) const;
    
    DJV_PRIVATE_COPY(djvImageView);
    
    struct Private;
    std::unique_ptr<Private> _p;
};

//@} // djvGuiWidget

