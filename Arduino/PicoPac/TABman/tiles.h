unsigned int tiles[19][5][5] = {
  {
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 }  // ╔ 0
  },
  {
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0 }  // ╔ 1
  },
  {
    { 0, 0, 0, 0, 0 },
    { 1, 1, 1, 0, 0 },
    { 1, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 }  // ╗ 2
  },
  {
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 0, 1, 1, 1 },
    { 0, 0, 0, 0, 0 } // ╚ 3
  },
  {
    { 0, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 0 },
    { 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0 } // ╝ 4
  },
  {
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0 } // ╠ 5
  },
  {
    { 0, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 }  // ╣ 6
  },
  {
    { 0, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0 }  // ╩ 7
  },
  {
    { 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0 }  // ╦ 8
  },
  {
    { 0, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0 } // ╬ 9
  },
  {
    { 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0 } // - 10
  },
  {
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 } // | 11
  },
  {
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0 } // von oben 12
  },
    {
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0 } // von unten 13
  },
    {
    { 0, 0, 0, 0, 0 },
    { 1, 1, 1, 0, 0 },
    { 1, 1, 1, 1, 0 },
    { 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0 } // von links 14
  },
    {
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 0, 1, 1, 1 },
    { 0, 0, 0, 0, 0 } // von rechts 15
  },
    {
    { 0, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 0, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0 } // | 16
  },
    {
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 } // | 17
  },
    {
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0 } // | 18
  },
};