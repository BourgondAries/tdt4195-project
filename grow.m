function [image grown] = grow(image, points)
	hmax = size(image, 1);
	vmax = size(image, 2);
	grown = zeros(hmax, vmax);
	while ~isempty(points)
		i = points(:, 1);
		points = points(:, 2:end);
		if image(i(1), i(2)) == 1
			image(i(1), i(2)) = 0;
			grown(i(1), i(2)) = 1;
			% Horizontal
			if i(1) ~= 1
				if image(i(1)-1, i(2)) == 1
					points = [points [i(1)-1; i(2)]];
				end
			end
			if i(1) ~= hmax
				if image(i(1)+1, i(2)) == 1
					points = [points [i(1)+1; i(2)]];
				end
			end

			% Vertical
			if i(2) ~= 1
				if image(i(1), i(2)-1) == 1
					points = [points [i(1); i(2)-1]];
				end
			end
			if i(2) ~= vmax
				if image(i(1), i(2)+1) == 1
					points = [points [i(1); i(2)+1]];
				end
			end

			% Diagonal choices
			if i(1) ~= 1 & i(2) ~= 1
				if image(i(1)-1, i(2)-1) == 1
					points = [points [i(1)-1; i(2)-1]];
				end
			end
			if i(1) ~= 1 & i(2) ~= vmax
				if image(i(1)-1, i(2)+1) == 1
					points = [points [i(1)-1; i(2)+1]];
				end
			end
			if i(1) ~= hmax & i(2) ~= 1
				if image(i(1)+1, i(2)-1) == 1
					points = [points [i(1)+1; i(2)-1]];
				end
			end
			if i(1) ~= hmax & i(2) ~= vmax
				if image(i(1)+1, i(2)+1) == 1
					points = [points [i(1)+1; i(2)+1]];
				end
			end
		end
	end
end
