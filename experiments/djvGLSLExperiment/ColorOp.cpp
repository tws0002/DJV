//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <ColorOp.h>

#include <Context.h>

#include <djvUI/FloatEditSlider.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>

#include <QGroupBox>
#include <QVBoxLayout>

using namespace gl;

//------------------------------------------------------------------------------
// ColorOp
//------------------------------------------------------------------------------

ColorOp::ColorOp(Context * context) :
    AbstractOp(context),
    _init(false)
{}

const ColorOp::Values & ColorOp::values() const
{
    return _values;
}

namespace
{
const QString vertexSource =
"void main(void)\n"
"{\n"
"    gl_FrontColor  = gl_Color;\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

const QString fragmentSource =
"vec4 colorFnc(vec4 value, mat4 color)\n"
"{\n"
"    vec4 tmp;\n"
"    tmp[0] = value[0];\n"
"    tmp[1] = value[1];\n"
"    tmp[2] = value[2];\n"
"    tmp[3] = 1.0;\n"
"    tmp *= color;\n"
"    tmp[3] = value[3];\n"
"    return tmp;\n"
"}\n"
"\n"
"uniform sampler2DRect texture;\n"
"uniform mat4 color;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = texture2DRect(texture, gl_TexCoord[0].st);\n"
"    gl_FragColor = colorFnc(gl_FragColor, color);\n"
"}\n";

} // namespace

void ColorOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("ColorOp::render");
    //DJV_DEBUG_PRINT("in = " << in);

    // Initialize.
    begin();
    _texture.init(in, GL_TEXTURE_RECTANGLE);
    if (! _init)
    {
        _shader.init(vertexSource, fragmentSource);
        _init = true;
    }

    // Render.
    _shader.bind();
    const float b = _values.brightness;
    const float c = _values.contrast;
    const float s = _values.saturation;
    const glm::mat4x4 value =
        djvOpenGLImageColor::brightnessMatrix(b, b, b) *
        djvOpenGLImageColor::contrastMatrix(c, c, c) *
        djvOpenGLImageColor::saturationMatrix(s, s, s);
    glUniformMatrix4fv(
        glGetUniformLocation(_shader.program(), "color"), 1, false,
        &value[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(_shader.program(), "texture"), 0);
    _texture.bind();
    const djvPixelDataInfo & info = in.info();
    djvOpenGLUtil::ortho(info.size);
    glViewport(0, 0, info.size.x, info.size.y);
    glClear(GL_COLOR_BUFFER_BIT);
    Util::quad(info);
    end();
}

void ColorOp::setValues(const Values & values)
{
    if (values == _values)
        return;
    _values = values;
    Q_EMIT opChanged();
}

//------------------------------------------------------------------------------
// ColorOpWidget
//------------------------------------------------------------------------------

ColorOpWidget::ColorOpWidget(
    ColorOp * op,
    Context * context) :
    AbstractOpWidget(context),
    _op(op)
{
    // Create the widgets.
    QGroupBox * brightnessGroup = new QGroupBox("Brightness");

    djvFloatEditSlider * brightness = new djvFloatEditSlider(context);
    brightness->setRange(0.f, 5.f);
    brightness->setDefaultValue(1.f);

    QGroupBox * contrastGroup = new QGroupBox("Contrast");

    djvFloatEditSlider * contrast = new djvFloatEditSlider(context);
    contrast->setRange(0.f, 5.f);
    contrast->setDefaultValue(1.f);

    QGroupBox * saturationGroup = new QGroupBox("Saturation");

    djvFloatEditSlider * saturation = new djvFloatEditSlider(context);
    saturation->setRange(0.f, 5.f);
    saturation->setDefaultValue(1.f);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);

    QVBoxLayout * vLayout = new QVBoxLayout(brightnessGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(brightness);
    layout->addWidget(brightnessGroup);

    vLayout = new QVBoxLayout(contrastGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(contrast);
    layout->addWidget(contrastGroup);

    vLayout = new QVBoxLayout(saturationGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(saturation);
    layout->addWidget(saturationGroup);

    layout->addStretch();

    // Initialize.
    brightness->setValue(op->values().brightness);
    contrast->setValue(op->values().contrast);
    saturation->setValue(op->values().saturation);

    // Setup the callbacks.
    connect(
        brightness,
        SIGNAL(valueChanged(float)),
        SLOT(brightnessCallback(float)));
    connect(
        contrast,
        SIGNAL(valueChanged(float)),
        SLOT(contrastCallback(float)));
    connect(
        saturation,
        SIGNAL(valueChanged(float)),
        SLOT(saturationCallback(float)));
}

void ColorOpWidget::brightnessCallback(float in)
{
    ColorOp::Values values = _op->values();
    values.brightness = in;
    _op->setValues(values);
}

void ColorOpWidget::contrastCallback(float in)
{
    ColorOp::Values values = _op->values();
    values.contrast = in;
    _op->setValues(values);
}

void ColorOpWidget::saturationCallback(float in)
{
    ColorOp::Values values = _op->values();
    values.saturation = in;
    _op->setValues(values);
}

//------------------------------------------------------------------------------
// ColorOpFactory
//------------------------------------------------------------------------------

ColorOpFactory::ColorOpFactory(Context * context) :
    AbstractOpFactory(context)
{}

AbstractOp * ColorOpFactory::createOp() const
{
    return new ColorOp(context());
}

AbstractOpWidget * ColorOpFactory::createWidget(
    AbstractOp * op) const
{
    return new ColorOpWidget(dynamic_cast<ColorOp *>(op), context());
}

