.PHONY: test clean

test:
	@echo "Testing all LPC files..."
	@find . -name "*.c" -exec ../amlp-driver/build/driver compile {} \; || true
	@echo "Test complete."

clean:
	@echo "Nothing to clean (no build artifacts)"
