function positions = computeAverageBlot(image)
	hmax = size(image, 1);
	vmax = size(image, 2);
	alreadySeen = zeros(hmax, vmax);
	positions = [];
	for x = 1:hmax
		for y = 1:vmax
			if alreadySeen(x, y) == 0
				if image(x, y) == 1
					[discard grown] = grow(image, [x; y]);
					alreadySeen = alreadySeen + grown;

					sums = [0; 0; 0];
					for i = 1:hmax
						for j = 1:vmax
							if grown(i, j) == 1
								sums = sums + [j; i; 1];
							end
						end
					end
					if sums(3) ~= 0
						sums = sums ./ sums(3);
						positions = [positions sums(1:2)];
					end
				end
			end
		end
	end
end
