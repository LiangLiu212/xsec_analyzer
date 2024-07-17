all: Slice_Plots

Slice_Plots: Slice_Plots.C 
	$(CXX) $(shell root-config --cflags --libs) -O3 -I$(STV_ANALYSIS_DIR)/Utils/Includes $(STV_ANALYSIS_DIR)/obj/*.o -o $(STV_ANALYSIS_DIR)/bin/$@ $^

clean:
	$(RM) Slice_Plots
	$(RM) *~
	$(RM) stv_root_dict_rdict.pcm
