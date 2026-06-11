#include "wordengine.h"
#include <QDir>

WordEngine::WordEngine(QObject *parent)
    : QObject{parent}
{
    m_wordApp = nullptr;
    m_documents = nullptr;
    m_activeDoc = nullptr;
    m_isOpen = false;
}

WordEngine::~WordEngine()
{
    close();
}

bool WordEngine::openTemplate(const QString &templatePath)
{
    QFileInfo fileInfo(templatePath);
    if (!fileInfo.exists()) {
        qDebug() << "模板文件不存在:" << templatePath;
        return false;
    }
    // 1. 初始化应用程序对象（Word 或 WPS）
    if (!initializeApplication()) {
        return false;
    }
    // 2. 设置为不可见（后台运行）
    m_wordApp->setProperty("Visible", false);
    // 3. 获取 Documents 集合
    m_documents = m_wordApp->querySubObject("Documents");
    if (!m_documents) {
        qDebug() << "无法获取 Documents 对象";
        close();
        return false;
    }
    // 4. 基于模板创建新文档（注意：WPS 和 Word 都支持该方法）
    QString path = QDir::toNativeSeparators(templatePath);
    QAxObject *newDoc = m_documents->querySubObject("Add(const QString&)", path);
    if (!newDoc || newDoc->isNull()) {
        qDebug() << "无法基于模板创建新文档";
        close();
        return false;
    }
    delete newDoc;
    // 5. 获取当前激活的文档
    m_activeDoc = m_wordApp->querySubObject("ActiveDocument");
    if (!m_activeDoc) {
        qDebug() << "无法获取 ActiveDocument";
        close();
        return false;
    }
    m_isOpen = true;
    qDebug() << "成功打开模板:" << templatePath << " 使用:" << m_appName;
    return true;
}

bool WordEngine::saveAs(const QString &savePath)
{
    if (!m_isOpen || !m_activeDoc) {
        qDebug() << "文档未打开，无法保存";
        return false;
    }
    QString path = QDir::toNativeSeparators(savePath);
    m_activeDoc->dynamicCall("SaveAs(const QString&)", path);
    // 保存后立即关闭文档，释放文件锁
    qDebug() << "文档已保存到:" << savePath;
    return true;
}

void WordEngine::close()
{
    if (m_activeDoc && m_isOpen) {
        m_activeDoc->dynamicCall("Close(bool)", false);
        delete m_activeDoc;
        m_activeDoc = nullptr;
    }
    if (m_wordApp) {
        m_wordApp->dynamicCall("Quit()");
        delete m_wordApp;
        m_wordApp = nullptr;
    }
    m_documents = nullptr;
    m_isOpen = false;
}

bool WordEngine::replaceBookmarkText(const QString &bookmarkName, const QString &text)
{
    if (!m_isOpen || !m_activeDoc) {
        qDebug() << "文档未打开，无法替换书签";
        return false;
    }
    QAxObject *bookmark = m_activeDoc->querySubObject("Bookmarks(QVariant)", bookmarkName);
    if (!bookmark || bookmark->isNull()) {
        qDebug() << "未找到书签:" << bookmarkName;
        if (bookmark) delete bookmark;
        return false;
    }
    bookmark->dynamicCall("Select(void)");
    QAxObject *range = bookmark->querySubObject("Range");
    if (range) {
        range->setProperty("Text", text);
        delete range;
    }
    delete bookmark;
    return true;
}

bool WordEngine::insertImageAtBookmark(const QString &bookmarkName, const QString &imagePath, double width, double height)
{
    if (!m_isOpen || !m_activeDoc) {
        qDebug() << "文档未打开，无法插入图片";
        return false;
    }
    QFileInfo imageInfo(imagePath);
    if (!imageInfo.exists()) {
        qDebug() << "图片文件不存在:" << imagePath;
        return false;
    }
    // 自动计算原始像素对应的磅值
    if (width <= 0 || height <= 0) {
        QImage img(imagePath);
        if (img.isNull()) {
            qDebug() << "无法读取图片原始尺寸:" << imagePath;
            return false;
        }
        // Word 默认 DPI = 96，磅值 = 像素 * (72 / 96) = 像素 * 0.75
        double scale = 72.0 / 96.0;
        if (width <= 0) width = img.width() * scale;
        if (height <= 0) height = img.height() * scale;
        qDebug() << "自动设置图片尺寸(磅):" << width << "x" << height;
    }
    QAxObject *bookmark = m_activeDoc->querySubObject("Bookmarks(QVariant)", bookmarkName);
    if (!bookmark || bookmark->isNull()) {
        qDebug() << "未找到书签:" << bookmarkName;
        if (bookmark) delete bookmark;
        return false;
    }
    QAxObject *bookmarkRange = bookmark->querySubObject("Range");
    if(!bookmarkRange){
        qDebug() << "无法获取书签的Range对象:" << bookmarkName;
        delete bookmark;
        return false;
    }
    // bookmark->dynamicCall("Select(void)");
    // 获取 InlineShapes 集合，添加图片
    QAxObject *inlineShapes = bookmarkRange->querySubObject("InlineShapes");
    if (inlineShapes) {
        QString imagePathNative = QDir::toNativeSeparators(imagePath);
        QAxObject *shape = inlineShapes->querySubObject("AddPicture(const QString&, bool, bool, QAxObject*)"
                                                        , imagePathNative, false, true, bookmarkRange->asVariant());
        if (shape && !shape->isNull()) {
            shape->setProperty("LockAspectRatio",true);
            shape->setProperty("Width", width);
            delete shape;
        }
        delete inlineShapes;
    }
    delete bookmarkRange;
    delete bookmark;
    return true;
}

bool WordEngine::initializeApplication()
{
    // 1. 直接尝试启动 Microsoft Word
    m_wordApp = new QAxWidget("Word.Application",nullptr,Qt::MSWindowsOwnDC);
    if (m_wordApp && !m_wordApp->isNull()) {
        m_appName = "Microsoft Word";
        qDebug() << "成功启动 Microsoft Word";
        return true;
    }
    // 2. 如果失败，清理并尝试 WPS
    delete m_wordApp;
    m_wordApp = new QAxWidget("KWPS.Application",nullptr,Qt::MSWindowsOwnDC);
    if (m_wordApp && !m_wordApp->isNull()) {
        m_appName = "WPS";
        qDebug() << "成功启动 WPS";
        return true;
    }
    // 3. 都失败了，清理资源并返回
    qDebug() << "既无法启动 Microsoft Word，也无法启动 WPS。请安装 Office 或 WPS。";
    delete m_wordApp;
    m_wordApp = nullptr;
    return false;
}
