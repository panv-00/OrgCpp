default:
	@+make -C build

clean:
	@rm -f build/*.o
	@rm -f orgcpp
	@echo "Clean!"
