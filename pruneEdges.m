function pixels = pruneEdges(image)
	pixels = [];
	% Top and bottom rows
	for i = 1:size(image(1, :), 2)
		if image(1, i) == 1
			pixels = [pixels; 1 i];
		end
	end
	low = size(image, 1);
	for i = 1:size(image(low, :), 2)
		if image(low, i) == 1
			pixels = [pixels; low i];
		end
	end

	% Left and right columns
	for i = 1:size(image(:, 1), 1)
		if image(i, 1) == 1
			pixels = [pixels; i 1];
		end
	end
	high = size(image, 2);
	for i = 1:size(image(:, high), 1)
		if image(i, high) == 1
			pixels = [pixels; i high];
		end
	end
	pixels = transpose(pixels);
end
