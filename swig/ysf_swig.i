/* -*- c++ -*- */

#define YSF_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ysf_swig_doc.i"

%{
#include "ysf/deframer_bb.h"
#include "ysf/fich_sink_b.h"
#include "ysf/dch_sink_b.h"
#include "ysf/vd2_vch_decoder_bb.h"
%}

%include "ysf/deframer_bb.h"
GR_SWIG_BLOCK_MAGIC2(ysf, deframer_bb);

%include "ysf/fich_sink_b.h"
GR_SWIG_BLOCK_MAGIC2(ysf, fich_sink_b);
%include "ysf/dch_sink_b.h"
GR_SWIG_BLOCK_MAGIC2(ysf, dch_sink_b);

%include "ysf/vd2_vch_decoder_bb.h"
GR_SWIG_BLOCK_MAGIC2(ysf, vd2_vch_decoder_bb);
