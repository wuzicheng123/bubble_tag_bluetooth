#ifndef WORDENGINE_H
#define WORDENGINE_H

#include <QObject>
#include <QAxObject>
#include <QAxWidget>

class WordEngine : public QObject
{
    Q_OBJECT
public:
    explicit WordEngine(QObject *parent = nullptr);

    ~WordEngine();

    // 基于模板创建新文档
    bool openTemplate(const QString &templatePath);
    // 保存文档
    bool saveAs(const QString &savePath);
    // 关闭文档并退出Word
    void close();

    // 替换文本书签
    bool replaceBookmarkText(const QString &bookmarkName, const QString &text);

    // 在书签处插入图片（从文件）
    bool insertImageAtBookmark(const QString &bookmarkName, const QString &imagePath,
                               double width = 0, double height = 0);

    // 获取当前使用的应用程序名称（Word/WPS）
    QString currentApplicationName() const { return m_appName; }

private:
    bool initializeApplication();   // 尝试创建 Word 或 WPS 实例
    QAxWidget *m_wordApp;      // Word应用程序实例
    QAxObject *m_documents;    // Documents集合
    QAxObject *m_activeDoc;    // 当前活动文档
    bool m_isOpen;             // 文档是否已打开
    QString m_appName;              // 记录成功启动的应用名称
};

#endif // WORDENGINE_H
