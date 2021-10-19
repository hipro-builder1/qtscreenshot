#include <QPixmap>
#include <QScreen>
#include <QTimer>

#include "app.h"

App::App(std::shared_ptr<spdlog::logger> logger, int argc, char **argv)
    : m_logger(logger), m_argc(argc), m_argv(argv), QApplication(argc, argv) {}

void App::init() {}

void App::free_to_capture() {
  auto no_of_screens = screens();
  m_logger->info("no_of_screens : {}", no_of_screens.size());
  // for (auto no : no_of_screens) {
  //   m_logger->info("{}", no.);
  QScreen *screen = primaryScreen();
  if (screen == nullptr) {
    m_logger->error("Couldn't get access to screen.");
    return;
  }

  QPixmap ss = screen->grabWindow(0);
  QString filename = "demo.jpg";
  if (ss.save(filename) == false) {
    m_logger->info("Couldn't save screenshot.");
    return;
  }

  m_logger->info("Captured ss");
}

void App::capture(std::function<void(std::vector<uint8_t> &&)> callback) {
  m_callback = callback;
  QTimer::singleShot(0, this, &App::free_to_capture);
}