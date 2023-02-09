default:
	@+make -C build

clean:
	@rm -f build/*.o
	@rm -f notescpp
	@echo "Clean!"
