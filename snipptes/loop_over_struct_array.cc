      octave_map m = tc.map_value ();

      //char buf[64];
      //int32_t maxfieldnamelength = max_namelen + 1;

      octave_idx_type nfields = m.nfields ();
      octave_stdout << "isstruct(), nfields = " << nfields << std::endl;

      string_vector keys = m.keys ();

      // loop over keys
      for (octave_idx_type i = 0; i < nfields; i++)
        {
          std::string key = keys(i);
           octave_stdout << "keys(" << i << ") = " << key << std::endl;

        }

      octave_idx_type len = m.numel ();
      octave_stdout << "len = " << len << std::endl;

      std::vector<const octave_value *> elts (nfields);
      for (octave_idx_type i = 0; i < nfields; i++)
        elts[i] = m.contents (keys(i)).data ();

      // loop over the elements
     for (octave_idx_type j = 0; j < len; j++)
      {
        // Iterating over the list of keys will preserve the order
        // of the fields.
        for (octave_idx_type i = 0; i < nfields; i++)
          {
            //octave_stdout << elts[i][j] << std::endl;
            //octave_stdout << elts[i][j].double_value() << std::endl;
            octave_stdout << elts[i][j].class_name() << std::endl;
            octave_stdout << elts[i][j].matrix_value() << std::endl;
          }
      }
