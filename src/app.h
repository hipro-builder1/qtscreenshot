#include <functional>
#include <memory>

#include <QApplication>
#include <QObject>

#include <spdlog/spdlog.h>

class App : public QApplication {
  Q_OBJECT
public:
  App(std::shared_ptr<spdlog::logger> logger, int argc, char **argv);

  void init();
  void capture(std::function<void(std::vector<uint8_t> &&)> callback);

private:
  void free_to_capture();

private:
  std::shared_ptr<spdlog::logger> m_logger;
  int m_argc;
  char **m_argv;

  std::function<void(std::vector<uint8_t> &&)> m_callback;
};