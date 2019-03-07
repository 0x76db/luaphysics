-- legge oraria
-- r(t) : R -> R x R
--        t -> (x, y)
g = 9.81;

-- velocita iniziale
v0x = 20.0;
v0y = 0.0;

-- posizione iniziale
x0 = 0
y0 = 10;

-- accelerazione
ax = 0
ay = -g

function r (t)
	x = x0 + v0x * t + 1/2*ax*t*t;
	y = y0 + v0y * t + 1/2*ay*t*t;
  
	return x, y
end