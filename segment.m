for i=1:3
	ix = num2str(i);

	image = imread(['image' ix '.png']);

	% Get the red checkers
	% RGB -> Grey
	gray = @(matrix) matrix(:, :, 1) * 0.2126 + ...
		matrix(:, :, 2) * 0.7152 + matrix(:, :, 3) * 0.0722;
	image = gray(image);
	imwrite(image, ['image' ix '-filter0.png']);

	% Now use some morphological closing
	circle = strel('disk', 25);
	image = imopen(image, circle);
	imwrite(image, ['image' ix '-filter1.png']);
	image0 = image;

	for j=1:2
		jx = num2str(j);
		image = image0;

		% Select all color values below a certain treshold
		if j == 1
			avg = mean(mean(image));
			image = image < avg * 0.6;
			imwrite(image, ['image' ix jx '-filter2.png']);
		elseif j == 2
			avg = mean(mean(image));
			image = image > avg * 1.6;
			imwrite(image, ['image' ix jx '-filter2.png']);
		end

		% Remove any small objects using opening
		image = imerode(image, circle);
		imwrite(image, ['image' ix jx '-filter3.png']);

		% Remove the edges, because they may be shaded wrongly
		pixels = pruneEdges(image);
		[image grown] = grow(image, pixels);
		imwrite(image, ['image' ix jx '-filter4.png']);

		% Now get the average of each cluster
		computeAverageBlot(image);

		imwrite(image, ['image' ix jx '-processed.png']);
	end
end
