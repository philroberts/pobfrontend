#include <QtGui/QOpenGLWindow>
#include <QRegularExpression>
#include <memory>

QPainter *painter = NULL;
QOpenGLTexture *white = NULL;

// Font alignment
enum r_fontAlign_e {
	F_LEFT,
	F_CENTRE,
	F_RIGHT,
	F_CENTRE_X,
	F_RIGHT_X
};

// Fonts
enum r_fonts_e {
	F_FIXED,	// Monospaced: Bitsteam Vera Sans Mono
	F_VAR,		// Normal: Liberation Sans
	F_VAR_BOLD,	// Normal: Liberation Sans Bold
	F_NUMFONTS
};

// Texture flags
enum r_texFlag_e {	
	TF_CLAMP	= 0x01,	// Clamp texture
	TF_NOMIPMAP	= 0x02,	// No mipmaps
	TF_NEAREST	= 0x04,	// Use nearest-pixel magnification instead of linear
	TF_ASYNC	= 0x08	// Asynchronous loading
};

class Cmd {
  public:
    virtual void execute() = 0;
};

class ViewportCmd : public Cmd {
  public:
  ViewportCmd(int X, int Y, int W, int H) : x(X), y(Y), w(W), h(H) {
    }

    void execute() {
        glViewport(x, painter->window().size().height() - y - h, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, (float)w, (float)h, 0, -9999, 9999);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
  private:
    int x, y, w, h;
};

class ColorCmd : public Cmd {
  public:
  ColorCmd(float Col[4]) : col {Col[0], Col[1], Col[2], Col[3]} {}
    void execute() {
        glColor4fv(col);
    }
  private:
    float col[4];
};

class DrawImageQuadCmd : public Cmd {
  public:
    DrawImageQuadCmd() {}
  DrawImageQuadCmd(QOpenGLTexture *Tex, float X0, float Y0, float X1, float Y1, float X2, float Y2, float X3, float Y3, float S0 = 0, float T0 = 0, float S1 = 1, float T1 = 0, float S2 = 1, float T2 = 1, float S3 = 0, float T3 = 1) : x {X0, X1, X2, X3}, y {Y0, Y1, Y2, Y3}, s {S0, S1, S2, S3}, t {T0, T1, T2, T3}, tex(Tex) {
    }

    void execute() {
        if (tex != NULL && tex->isCreated()) {
            tex->bind();
        } else {
            white->bind();
        }
        glBegin(GL_TRIANGLE_FAN);
        for (int v = 0; v < 4; v++) {
            glTexCoord2d(s[v], t[v]);
            glVertex2d(x[v], y[v]);
        }
        glEnd();
    }
  protected:
    QOpenGLTexture *tex;
    float x[4];
    float y[4];
    float s[4];
    float t[4];
};

class DrawImageCmd : public DrawImageQuadCmd {
  public:
  DrawImageCmd(QOpenGLTexture *tex, float x, float y, float w, float h, float s1 = 0, float t1 = 0, float s2 = 1.0f, float t2 = 1.0f) : DrawImageQuadCmd(tex, x, y, x + w, y, x + w, y + h, x, y + h, s1, t1, s2, t1, s2, t2, s1, t2) {
    }
};

class DrawStringCmd : public DrawImageQuadCmd {
  public:
  DrawStringCmd(float X, float Y, int Align, int Size, int Font, const char *Text) : text(Text) {
        if (text[0] == '^') {
            switch(text[1].toLatin1()) {
            case '0':
                setCol(0.0f, 0.0f, 0.0f);
                break;
            case '1':
                setCol(1.0f, 0.0f, 0.0f);
                break;
            case '2':
                setCol(0.0f, 1.0f, 0.0f);
                break;
            case '3':
                setCol(0.0f, 0.0f, 1.0f);
                break;
            case '4':
                setCol(1.0f, 1.0f, 0.0f);
                break;
            case '5':
                setCol(1.0f, 0.0f, 1.0f);
                break;
            case '6':
                setCol(0.0f, 1.0f, 1.0f);
                break;
            case '7':
                setCol(1.0f, 1.0f, 1.0f);
                break;
            case '8':
                setCol(0.7f, 0.7f, 0.7f);
                break;
            case '9':
                setCol(0.4f, 0.4f, 0.4f);
                break;
            case 'x':
                int xr, xg, xb;
                sscanf(text.toStdString().c_str() + 2, "%2x%2x%2x", &xr, &xg, &xb);
                col[0] = xr / 255.0f;
                col[1] = xg / 255.0f;
                col[2] = xb / 255.0f;
                col[3] = 1.0f;
                break;
            default:
                break;
            }
        } else {
            col[3] = 0;
        }
        int count = 0;
        for (QRegularExpressionMatchIterator i = QRegularExpression("(\\^x.{6})|(\\^\\d)").globalMatch(text);i.hasNext();i.next()) {
            count += 1;
        }
        if (count > 1) {
            //std::cout << text.toStdString().c_str() << " " << count << std::endl;
        }
        text.remove(QRegularExpression("(\\^x.{6})|(\\^\\d)"));

        QFont font("Inconsolata", Size - 5);
        QFontMetrics fm(font);
        Y += fm.height() * 0.9;
        double width = fm.width(text);
        switch (Align) {
	case F_CENTRE:
            X = floor((painter->window().size().width() - width) / 2.0f + X);
            break;
        case F_RIGHT:
            X = floor(painter->window().size().width() - width - X);
            break;
        case F_CENTRE_X:
            X = floor(X - width / 2.0f);
            break;
        case F_RIGHT_X:
            X = floor(X - width) + 5;
            break;
        }

        QRect br = fm.boundingRect(text);
        QImage brush(fm.boundingRect(text).size(), QImage::Format_ARGB32);
        brush.fill(QColor(0, 0, 0, 0));
        tex = NULL;
        if (brush.width() && brush.height()) {
            QPainter p(&brush);
            p.setPen(QColor(255, 255, 255, 255));
            p.setFont(font);
            p.drawText(br.left(), -br.top(), text);
            p.end();
            tex = new QOpenGLTexture(brush);
        }
        x[0] = X;
        y[0] = Y - brush.height();
        x[1] = X + brush.width();
        y[1] = Y - brush.height();
        x[2] = X + brush.width();
        y[2] = Y;
        x[3] = X;
        y[3] = Y;

        s[0] = 0;
        t[0] = 0;
        s[1] = 1;
        t[1] = 0;
        s[2] = 1;
        t[2] = 1;
        s[3] = 0;
        t[3] = 1;
    }

    ~DrawStringCmd() {
        if (tex != NULL) {
            delete tex;
        }
    }

    void execute() {
        float curCol[4];
        if (col[3] > 0) {
            glGetFloatv(GL_CURRENT_COLOR, curCol);
            glColor4fv(col);
        }
        DrawImageQuadCmd::execute();
        if (col[3] > 0) {
            glColor4fv(curCol);
        }
    }

    void setCol(float c0, float c1, float c2) {
        col[0] = c0;
        col[1] = c1;
        col[2] = c2;
        col[3] = 1.0f;
    }
  private:
    float col[4];
    QString text;
};

class POBWindow : public QOpenGLWindow {
    //    Q_OBJECT
public:
//    POBWindow(QWindow *parent = 0) : QOpenGLWindow(parent) {};
    POBWindow() {
//        QSurfaceFormat theformat(format());
//        format.setProfile(QSurfaceFormat::CompatibilityProfile);
/*        format.setDepthBufferSize(24);
        format.setStencilBufferSize(0);
        format.setGreenBufferSize(8);
        format.setRedBufferSize(8);
        format.setBlueBufferSize(8);*/
//        theformat.setAlphaBufferSize(8);
//        std::cout << theformat.hasAlpha() << std::endl;
//        setFormat(theformat);
    }

//    POBWindow() : QOpenGLWindow() {
//    };
//    ~POBWindow() {};
//protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void LAssert(lua_State* L, int cond, const char* fmt, ...);
    int IsUserData(lua_State* L, int index, const char* metaName);

    void SetDrawLayer(int layer);
    void SetDrawLayer(int layer, int subLayer);
    void SetDrawSubLayer(int subLayer) {
        SetDrawLayer(curLayer, subLayer);
    }
    void AppendCmd(std::shared_ptr<Cmd> cmd);
    void DrawColor(const float col[4] = NULL);
    void DrawColor(uint32_t col);
    QString scriptWorkDir;
    int curLayer;
    int curSubLayer;
    float drawColor[4];
    QMap<QPair<int, int>, QList<std::shared_ptr<Cmd>>> layers;
};
