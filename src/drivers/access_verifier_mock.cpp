#include "drivers/access_verifier.hpp"
#include "core/logger.hpp"

class AccessVerifierMock : public AccessVerifier {
public:
    void start(BlockingQueue<Event>&) override {
        log_line("[MOCK] AccessVerifier ready (input handled by CommandInput)");
    }

    void stop() override {
        log_line("[MOCK] AccessVerifier stopped.");
    }
};

AccessVerifier* create_access_verifier_mock() {
    return new AccessVerifierMock();
}